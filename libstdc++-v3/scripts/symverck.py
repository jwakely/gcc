#!/usr/bin/python3

from __future__ import print_function
import sys
import fnmatch
import subprocess
import argparse

GLOB, DEMANGLED = 1, 2

def read_patterns(filename):
	"""Read a linker script into a dictionary.

	Returns { "VER_1": {N:("foo*", T), N:("bar[ij]", T)} }
	where N is the line number of the pattern
	and T is a bitmask of the elements GLOB and DEMANGLED.
	"""
	with open(filename) as f:
		INIT, VERSION, EXTERN_CXX = range(3)
		patterns = {}
		filename = filename.split('/')[-1]
		state = INIT
		ver = None
		local = False
		for n, line in enumerate(f):
			# trim comments
			line = line.partition('#')[0].strip()
			# skip blank lines
			if not line:
				continue
			n += 1
			if state == INIT:
				if line[-1] != '{':
					raise ValueError("{0}:{1}: Invalid characters at global scope: {2}".format(filename, n, line))
				ver = line[0:-1].rstrip()
				state = VERSION
				local = False
				continue
			elif state == VERSION:
				if line == "global:":
					continue
				elif line == "local:":
					local = True
					continue
				if line == 'extern "C++"':
					state = EXTERN_CXX
					continue
				if line[0] == '}' and line[-1] == ';':
					state = INIT
					continue
			elif state == EXTERN_CXX:
				if line == '{':
					continue
				elif line == '};':
					state = VERSION
					continue

			if line[-1] != ';':
				# TODO allow trailing semi-colon to be omitted at end of node
				raise ValueError("{0}:{1}: Invalid pattern: {2}".format(filename, n, line))

			if local:
				continue

			pattern = line[0:-1]
			pattern_type = 0
			if state == EXTERN_CXX:
				pattern_type |= DEMANGLED
			if pattern[0] == '"' and pattern[-1] == '"':
				pattern = pattern[1:-1]
			else:
				pattern_type |= GLOB
			patterns.setdefault(ver, {})[n] = (pattern, pattern_type)

		if state != INIT:
			raise ValueError("{0}: incomplete node: {1}".format(filename, ver))

		return patterns

class Demangler:
	def __enter__(self):
		self.child = subprocess.Popen(['c++filt'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.child.stdin.close()

	def __call__(self, mangled):
		"""Demangle a symbol name."""
		self.child.stdin.write((mangled+'\n').encode())
		self.child.stdin.flush()
		return self.child.stdout.readline().decode().rstrip('\n')

def find_matches(symbols, patterns, verbose, full, conflicts = False):
	"""For each symbol name find which patterns it matches.

	Return a list of symbols and the patterns they match.

	When the optional argument conflicts is True only match symbols
	that match patterns in more than one version.
	"""
	with Demangler() as demangle:
		res = []
		count = 0
		for symbol in symbols:
			demangled = demangle(symbol)

			matches = {}

			for v, ps in iter(patterns.items()):
				for n, (p, t) in iter(ps.items()):
					if t & DEMANGLED:
						s = demangled
					else:
						s = symbol
					# Python fnmatch uses [!abc] not [^abc]
					p = p.replace('[^', '[!')
					if fnmatch.fnmatch(s, p):
						matches.setdefault(v, []).append(n)
						if not full:
							break

			if symbol == demangled:
				demangled = None

			if conflicts:
				if len(matches) > 1:
					res.append( (symbol, demangled, matches) )
					if verbose:
						sys.stdout.write('!')
						sys.stdout.flush()
			else:
				res.append( (symbol, demangled, matches) )

			if verbose:
				count += 1
				if not (count % 10):
					sys.stdout.write('#')
					sys.stdout.flush()
		if verbose:
			sys.stdout.write('\n')

		return res


def show_matches(all_matches, patterns, conflicts):
	for symbol, demangled, matches in all_matches:
		if len(matches) == 0:
			continue
		if conflicts:
			print("Symbol matches more than one version:")
		else:
			print("Symbol matches:")
		print(symbol)
		if demangled:
			print(demangled)
		for v, ms in iter(matches.items()):
			for m in ms:
				p = patterns[v][m][0]
				print("  {0:<18} {1} (line {2})".format(v, p, m))
		else:
			print('')

def main():
	parser = argparse.ArgumentParser(description='Read symbols from standard input and match against patterns in a linker script.')
	parser.add_argument('script', metavar='linker-script', help='linker version script')
	parser.add_argument('-c', '--conflicts', action = 'store_true', help='check for symbols that match multiple patterns')
	parser.add_argument('-u', '--unmatched', action = 'store_true', help='check for symbols that match no patterns')
	parser.add_argument('-f', '--full', action = 'store_true', help='check all patterns for a version (default: only report first match per version)')
	parser.add_argument('-v', '--verbose', action = 'store_true', help='show progress indicator every ten symbols processed')

	args = parser.parse_args()

	patterns = read_patterns(args.script)
    # N.B. for python2.7 need to send EOF twice when stdin is a tty:
    # https://bugs.python.org/issue15068
	symbols = [line.strip() for line in sys.stdin]
	matches = find_matches(symbols, patterns, args.verbose, args.full and not args.unmatched, args.conflicts)

	if args.unmatched:
		for symbol in [s for s, _, m in matches if len(m) == 0]:
			print(symbol)
	else:
		show_matches(matches, patterns, args.conflicts)

if __name__ == "__main__": main()
