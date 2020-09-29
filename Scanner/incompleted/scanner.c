/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank()
{
  // TODO
  while ((currentChar != EOF) &&
         (charCodes[currentChar] == CHAR_SPACE))
    readChar();
}

void skipComment()
{
  // TODO
  int ln = lineNo;
  int cn = colNo;
  readChar();
  if (currentChar == EOF)
    error(ERR_ENDOFCOMMENT, ln, cn);
  while (1)
  {
    if (charCodes[currentChar] == CHAR_TIMES)
    {
      readChar();
      if (currentChar == EOF)
        error(ERR_ENDOFCOMMENT, ln, cn);
      if (charCodes[currentChar] == CHAR_RPAR)
      {
        readChar();
        return;
      }
    }
    if (currentChar == EOF )
      error(ERR_ENDOFCOMMENT, ln, cn);
    readChar();
  }
}

Token *readIdentKeyword(void)
{
  char s[MAX_IDENT_LEN + 1];
  int count = 0;
  int cn = colNo, ln = lineNo;
  Token *token = makeToken(TK_IDENT, ln, cn);
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
  {
    s[count] = (char)currentChar;
    count++;
    readChar();
  }
  s[count] = '\0';
  if (count > MAX_IDENT_LEN)
    error(ERR_IDENTTOOLONG, ln, cn);
  if (checkKeyword(s) != TK_NONE)
  {
    token = makeToken(checkKeyword(s), ln, cn);
  }
  strcpy(token->string, s);
  return token;
}

Token *readNumber(void)
{
  // TODO
  int cn = colNo, ln = lineNo;
  int number = currentChar - '0';
  readChar();
  while (1)
  {
    if (charCodes[currentChar] == CHAR_DIGIT)
    {
      int tmp = number * 10 + currentChar - '0';
      if (tmp < number)
        error(ERR_NUMBERTOOBIG, ln, cn);
      number = tmp;
      readChar();
    }
    else
    {
      Token *token = makeToken(TK_NUMBER, ln, cn);
      sprintf(token->string, "%d", number);
      return token;
    }
  }
}

Token *readConstChar(void)
{
  // TODO
  int ln = lineNo, cn = colNo;
  Token *token = makeToken(TK_CHAR, ln, cn);
  readChar();
  // printf("current char %d\n", currentChar);
  if (currentChar == EOF)
  {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
  }
  token->string[0] = (char)currentChar;
  token->string[1] = '\0';
  readChar();
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
  {
    readChar();
    return token;
  }
  else
  {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
  }
}

Token *getToken(void)
{
  Token *token;
  int ln = lineNo, cn = colNo;
  // printf("--- current char --- %d\n", currentChar);
  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, ln, cn);
    readChar();
    return token;

  case CHAR_MINUS:
    token = makeToken(SB_MINUS, ln, cn);
    readChar();
    return token;

  case CHAR_TIMES:
    token = makeToken(SB_TIMES, ln, cn);
    readChar();
    return token;

  case CHAR_SLASH:
    token = makeToken(SB_SLASH, ln, cn);
    readChar();
    return token;

  case CHAR_EQ:
    token = makeToken(SB_EQ, ln, cn);
    readChar();
    return token;

  case CHAR_COMMA:
    token = makeToken(SB_COMMA, ln, cn);
    readChar();
    return token;

  case CHAR_SEMICOLON:
    readChar();
    return makeToken(SB_SEMICOLON, ln, cn);

  case CHAR_RPAR:
    readChar();
    return makeToken(SB_RPAR, ln, cn);

  case CHAR_GT:
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_GE, ln, cn);
    }
    else
    {
      token = makeToken(SB_GT, ln, cn);
    }
    return token;

  case CHAR_LT:
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_LE, ln, cn);
    }
    else
    {
      token = makeToken(SB_LT, ln, cn);
    }
    return token;

  case CHAR_EXCLAIMATION:
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_NEQ, ln, cn);
    }
    else
    {
      error(ERR_INVALIDSYMBOL, ln, cn);
    }
    return token;

  case CHAR_COLON:
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_ASSIGN, ln, cn);
    }
    else
    {
      token = makeToken(SB_COLON, ln, cn);
    }
    return token;

  case CHAR_PERIOD:
    readChar();
    if (charCodes[currentChar] == CHAR_RPAR)
    {
      readChar();
      token = makeToken(SB_RSEL, ln, cn);
    }
    else
    {
      token = makeToken(SB_PERIOD, ln, cn);
    }
    return token;
  case CHAR_LPAR:
    readChar();
    switch (charCodes[currentChar])
    {
    case CHAR_PERIOD:
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    case CHAR_TIMES:
      skipComment();
      return getToken();
    default:
      return makeToken(SB_LPAR, ln, cn);
    }
  case CHAR_SINGLEQUOTE:
    return readConstChar();
    // ....
    // TODO
    // ....
  default:
    token = makeToken(TK_NONE, ln, cn);
    error(ERR_INVALIDSYMBOL, ln, cn);
    readChar();
    return token;
  }
}

/******************************************************************/

void printToken(Token *token)
{

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
