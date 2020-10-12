

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
        printf("return: char %c\n", (char)currentChar);
        return;
      }
    }
    if (currentChar == EOF)
      error(ERR_ENDOFCOMMENT, ln, cn);
    readChar();
  }
}

Token *token;
int state = 0;
int ln, cn;
char s[MAX_IDENT_LEN + 1];
int count, number;
int tmp = 0;

Token *getToken(void)

{
  ln = lineNo, cn = colNo;
  state = 0;
  s[0] = '\0';
  count = 0;
  number = currentChar - '0';
  while (1)
  {
    switch (state)
    {
    case 0:
      if (currentChar == EOF)
      {
        state = 37;
        break;
      }
      switch (charCodes[currentChar])
      {
      case CHAR_SPACE:
        state = 1;
        break;
      case CHAR_LETTER:
        state = 8;
        break;
      case CHAR_DIGIT:
        state = 10;
        break;
      case CHAR_PLUS:
        state = 12;
        break;

      case CHAR_MINUS:
        state = 13;
        break;

      case CHAR_TIMES:
        state = 14;
        break;

      case CHAR_SLASH:
        state = 15;
        break;

      case CHAR_EQ:
        state = 16;
        break;

      case CHAR_COMMA:
        state = 17;
        break;

      case CHAR_SEMICOLON:
        state = 18;
        break;

      case CHAR_RPAR:
        state = 39;
        break;

      case CHAR_GT:
        state = 22;
        break;

      case CHAR_LT:
        state = 25;
        break;

      case CHAR_EXCLAIMATION:
        state = 28;
        break;

      case CHAR_COLON:
        state = 31;
        break;

      case CHAR_PERIOD:
        state = 19;
        break;
      case CHAR_LPAR:
        state = 2;
        break;
      case CHAR_SINGLEQUOTE:
        state = 34;
        break;
      default:
        state = 38;
        break;
      }
      break;
    case 1:
      skipBlank();
      return getToken();
    case 2:
      readChar();
      switch (charCodes[currentChar])
      {
      case CHAR_PERIOD:
        state = 6;
        break;
      case CHAR_TIMES:
        state = 3;
        break;
      default:
        state = 7;
        break;
      }
      break;
    case 3:
      readChar();
      if (currentChar == EOF)
        state = 40;
      if (charCodes[currentChar] == CHAR_TIMES)
        state = 4;
      else
        state = 3;
      break;
    case 4:
      readChar();
      if (currentChar == EOF)
        state = 40;
      switch (charCodes[currentChar])
      {
      case CHAR_TIMES:
        state = 4;
        break;
      case CHAR_RPAR:
        state = 5;
        break;
      }
      break;
    case 5:
      readChar();
      return getToken();
    case 6:
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    case 7:
      return makeToken(SB_LPAR, ln, cn);
    case 8:
      s[count] = (char)currentChar;
      readChar();
      if (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
      {
        count++;
        state = 8;
      }
      else
      {
        s[count + 1] = '\0';
        state = 9;
      }
      break;
    case 9:
      if (count >= MAX_IDENT_LEN)
        error(ERR_IDENTTOOLONG, ln, cn);
      if (checkKeyword(s) != TK_NONE)
        return makeToken(checkKeyword(s), ln, cn);
      token = makeToken(TK_IDENT, ln, cn);
      strcpy(token->string, s);
      return token;
    case 10:
      tmp = number * 10 + currentChar - '0';
      readChar();
      if (charCodes[currentChar] == CHAR_DIGIT)
      {
        if (tmp < number)
          error(ERR_NUMBERTOOBIG, ln, cn);
        number = tmp;
        state = 10;
      }
      else
      {
        state = 11;
      }
      break;
    case 11:
      token = makeToken(TK_NUMBER, ln, cn);
      sprintf(token->string, "%d", number);
      return token;
    case 12:
      readChar();
      return makeToken(SB_PLUS, ln, cn);
    case 13:
      readChar();
      return makeToken(SB_MINUS, ln, cn);
    case 14:
      readChar();
      return makeToken(SB_TIMES, ln, cn);
    case 15:
      readChar();
      return makeToken(SB_SLASH, ln, cn);
    case 16:
      readChar();
      return makeToken(SB_EQ, ln, cn);
    case 17:
      readChar();
      return makeToken(SB_COMMA, ln, cn);
    case 18:
      readChar();
      return makeToken(SB_SEMICOLON, ln, cn);
    case 19:
      readChar();
      if (currentChar == EOF)
        return makeToken(TK_EOF, lineNo, colNo);
      switch (charCodes[currentChar])
      {
      case CHAR_RPAR:
        state = 20;
        break;
      default:
        state = 21;
        break;
      }
      break;
    case 20:
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    case 21:
      return makeToken(SB_PERIOD, ln, cn);
    case 22:
      readChar();
      if (charCodes[currentChar] == CHAR_EQ)
        state = 23;
      else
        state = 24;
      break;
    case 23:
      readChar();
      return makeToken(SB_GE, ln, cn);
    case 24:
      return makeToken(SB_GT, ln, cn);
    case 25:
      readChar();
      if (currentChar == EOF)
        return makeToken(TK_EOF, lineNo, colNo);
      switch (charCodes[currentChar])
      {
      case CHAR_EQ:
        state = 26;
        break;
      default:
        state = 27;
        break;
      }
      break;
    case 26:
      readChar();
      return makeToken(SB_LE, ln, cn);
    case 27:
      return makeToken(SB_LT, ln, cn);
    case 28:
      readChar();
      if (currentChar == EOF)
        return makeToken(TK_EOF, lineNo, colNo);
      switch (charCodes[currentChar])
      {
      case CHAR_EQ:
        state = 29;
        break;
      default:
        state = 30;
        break;
      }
      break;
    case 29:
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    case 30:
      readChar();
      error(ERR_INVALIDSYMBOL, ln, cn);
    case 31:
      readChar();
      if (currentChar == EOF)
        return makeToken(TK_EOF, lineNo, colNo);
      switch (charCodes[currentChar])
      {
      case CHAR_EQ:
        state = 32;
        break;
      default:
        state = 33;
        break;
      }
      break;
    case 32:
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    case 33:
      return makeToken(SB_COLON, ln, cn);
    case 34:
      readChar();
      if (currentChar == EOF)
      {
        error(ERR_INVALIDCHARCONSTANT, ln, cn);
        return makeToken(TK_NONE, ln, cn);
      }
      else if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
        state = 36;
      else
        state = 35;
      break;
    case 35:
      readChar();
      if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
      {
        state = 36;
      }
      break;
    case 36:
      readChar();
      return makeToken(TK_CHAR, ln, cn);
    case 37:
      readChar();
      return makeToken(TK_EOF, ln, cn);
    case 38:
      readChar();
      error(ERR_INVALIDSYMBOL, ln, cn);
    case 39:
      readChar();
      return makeToken(SB_RPAR, ln, cn);
    case 40:
      token = makeToken(TK_EOF, ln, cn);
      error(ERR_ENDOFCOMMENT, ln, cn);
      return token;
    default:
      error(ERR_INVALIDSYMBOL, ln, cn);
      readChar();
      return makeToken(TK_NONE, ln, cn);
    }
  }
}

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