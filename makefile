CC=clang
CFLAGS=-lc --debug
SRC_DIR = ./src/
SOURCES = ${SRC_DIR}*.c

debug: remove_parser parser
	./parser

compile: 
	${CC} ${CFLAGS} ${SOURCES}
	
parserGenerator: remove_parser_generator
	${CC} ${CFLAGS} \
	${SRC_DIR}lexer.c \
	${SRC_DIR}parserGeneratorUtils.c \
	${SRC_DIR}firstset.c \
	${SRC_DIR}followset.c \
	${SRC_DIR}parsingTable.c \
	${SRC_DIR}parserGenerator.c \
	-o parserGenerator
	
parser:
	${CC} ${CFLAGS} ${SRC_DIR}parser.c ${SRC_DIR}lexer.c ${SRC_DIR}parserGeneratorUtils.c -o parser

remove_parser:
	rm -f parser
	
remove_parser_generator:
	rm -f parserGenerator

clean: remove_parser remove_parser_generator
	