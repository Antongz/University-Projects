include Makefile-extras

# must specify make target
all: test

notest: tokeniser

# C++ 11
CXXFLAGS=--std=c++11 -I. -Iincludes

tokeniser.cpp: includes/tokeniser.h

tokeniser: tokens.cpp tokeniser.cpp lib/lib.a
	${CXX} ${CXXFLAGS} -o $@ $^

clean:
	-@rm -f tokeniser *.o

# regenerate all expected test outputs using working tokeniser
test-new:
	-@bash bin/run-tests.bash new loud

# run tests using the working tokeniser - they should all pass!
test-working:
	-@bash bin/run-tests.bash working loud

# generate any missing expected test outputs using working tokeniser
test-add:
	-@bash bin/run-tests.bash add loud

# regenerate all expected test outputs using working tokeniser - quietly
test-new-quiet:
	-@bash bin/run-tests.bash new quiet

# generate any missing expected test outputs using working tokeniser - quietly
test-add-quiet:
	-@bash bin/run-tests.bash add quiet

# run tests using the working tokeniser - they should all pass! - quietly
test-working-quiet:
	-@bash bin/run-tests.bash working quiet

# run tests using the student tokeniser
test-tokeniser: test
test: tokeniser
	-@bash bin/run-tests.bash tokeniser loud

# run tests using the student tokeniser - quietly
test-tokeniser-quiet: test-quiet
test-quiet: tokeniser
	-@bash bin/run-tests.bash tokeniser quiet
