#
# Test batch file for a probabilistic Earley parser 
#

# First non-empty non-comment line of the file shoud be 
# a statement 
#    Type X
# where X is either D or P for Detereministic input or a
# Probabilistic input batch respectively

Type P

# Then there goes a list of files with test sequences - one per
# file. It essentially works the same way as it would if we ran the
# corresponding type parser once on each sequence.  
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq
/u/yivanov/dev/c/Aha/SParser/Tests/pncalc.seq






