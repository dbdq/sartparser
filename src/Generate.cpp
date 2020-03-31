#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#if defined(REAL_DOUBLE)
 #define m_type double
#else
 #define m_type float
#endif

#include <globdefs.h>
#include "Token.h"
#include "Production.h"
#include "SGrammar.h"

#define GRAMMAR_PATH       ".//Grammars//"
#define DEFAULT_GRAMMAR    "sl0.grm"
#define TEST_PATH          ".//Tests//"
#define DEFAULT_TEST       "l0.seq"

void PrintNT(PSGrammar G, PToken pT)
{
   T_REAL P = (T_REAL)0.0;

   P = (T_REAL)rand()/(RAND_MAX);

   PProduction pP = G->GetProduction(pT);
   T_REAL CumP = (T_REAL)0.0;
   
   PSRule pR = (PSRule)NULL;
   for(int i = 0; i < pP->GetRuleCount(); i++)
   {
      pR = (PSRule)pP->GetRule(i);
      
      CumP += pR->GetProb();
      if(CumP >= P)
	 break;
   }
   
   PTokItem pTI = pR->GetFirst();
   while(pTI)
   {
      PToken pNewT = pTI->GetToken();
      if(pNewT->GetType() == TOKEN_TERMINAL)
	 printf("%s ", pNewT->GetName());
      else
	 PrintNT(G, pNewT);
      pTI = pTI->GetNext();
   }
}

void GenerateString(PSGrammar G)
{
   struct timeval  timeofday;
   struct timezone timez;
   gettimeofday(&timeofday, &timez);


   srand(timeofday.tv_usec);
   PToken pT = G->GetAxiom();
   PrintNT(G, pT);
   printf("\n");
}

int main(int argc, char **argv)
{
   char GrammarName[256];

   if(argc > 1)
      sprintf(GrammarName, "%s%s", GRAMMAR_PATH, argv[1]);
   else
      sprintf(GrammarName, "%s%s", GRAMMAR_PATH, DEFAULT_GRAMMAR);

   PSGrammar G = new SGrammar();
   G->ReadGrammar(GrammarName);
   GenerateString(G);
      
   return 0;
}






