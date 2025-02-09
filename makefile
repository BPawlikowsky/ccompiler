CC=clang
CFLAGS=-lc --debug
SRC_DIR = ./src/
TOOLS_DIR = ./src/tools/
LL1_DIR = ./src/LL1/
UTILS_DIR = ./src/utils/
SOURCES = ${SRC_DIR}*.c
PARSER_DEPS = 	${SRC_DIR}memory.c \
	${TOOLS_DIR}logger.c \
	${TOOLS_DIR}lexer.c \
	${UTILS_DIR}utils.c \
	${LL1_DIR}parserGeneratorUtils.c \
	${LL1_DIR}LL1parser.c

LL1_PARSER_GEN_DEPS = 	${SRC_DIR}memory.c \
	${TOOLS_DIR}lexer.c \
	${LL1_DIR}parserGeneratorUtils.c \
	${TOOLS_DIR}firstset.c \
	${TOOLS_DIR}followset.c \
	${LL1_DIR}parsingTable.c \
	${TOOLS_DIR}logger.c \
	${TOOLS_DIR}settools.c \
	${UTILS_DIR}utils.c \
	${LL1_DIR}LL1Generator.c



debug: remove_parser LL1parser
	./LL1parser

compile: 
	${CC} ${CFLAGS} ${SOURCES}
	
LL1Generator: ${LL1_PARSER_GEN_DEPS}
	${CC} ${CFLAGS} \
	-o LL1Generator \
	${LL1_PARSER_GEN_DEPS}
	
LL1parser: ${PARSER_DEPS}
	${CC} ${CFLAGS} \
	-o LL1parser \
	${PARSER_DEPS}

remove_parser:
	rm -f LL1parser
	
remove_parser_generator:
	rm -f LL1Generator

clean: remove_parser remove_parser_generator
	
