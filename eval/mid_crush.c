#include "bbattery.h"
#include "config.h"
#include "scomp.h"
#include "sknuth.h"
#include "smarsa.h"
#include "smultin.h"
#include "snpair.h"
#include "sres.h"
#include "sspectral.h"
#include "sstring.h"
#include "svaria.h"
#include "swalk.h"
#include "swrite.h"
#include "ufile.h"
#include "unif01.h"
#include "util.h"

#include "chrono.h"
#include "fbar.h"
#include "fdist.h"
#include "gofs.h"
#include "gofw.h"
#include "num.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LEN 120
#define NAMELEN 30
#define NDIM 200 /* Dimension of extern arrays */
#define THOUSAND 1000
#define MILLION (THOUSAND * THOUSAND)
#define BILLION (THOUSAND * MILLION)

/* The number of tests in each battery */
#define SMALLCRUSH_NUM 10
#define MIDCRUSH_NUM 20
#define CRUSH_NUM 96
#define BIGCRUSH_NUM 106
#define RABBIT_NUM 26
#define ALPHABIT_NUM 9

static char CharTemp[LEN + 1];

/* Gives the test number as enumerated in bbattery.tex. Some test applies
   more than one test, so the array of p-values does not correspond with
   the test number in the doc. */
static int TestNumber[1 + NDIM] = {0};

/*-------------------------------- Functions ------------------------------*/

static void GetName(unif01_Gen *gen, char *genName) {
  char *p;
  int len1, len2;

  if (NULL == gen) {
    genName[0] = '\0';
    return;
  }

  /* Print only the generator name, without the parameters or seeds. */
  /* The parameters start after the first blank; name ends with ':' */
  genName[LEN] = '\0';
  len1 = strcspn(gen->name, ":");
  len1 = util_Min(LEN, len1);
  strncpy(genName, gen->name, (size_t)len1);
  genName[len1] = '\0';
  /* For Filters or Combined generators */
  p = strstr(&gen->name[1 + len1], "unif01");
  while (p != NULL) {
    len1 += 2;
    if (len1 >= LEN)
      return;
    strcat(genName, ", ");
    len2 = strcspn(p, " \0");
    len2 = util_Min(LEN - len1, len2);
    if (len2 <= 0)
      return;
    strncat(genName, p, (size_t)len2);
    len1 = strlen(genName);
    genName[len1] = '\0';
    p += len2;
    p = strstr(p, "unif01");
  }
}

/*=========================================================================*/

static void WritepVal(double p)
/*
 * Write a p-value with a nice format.
 */
{
  if (p < gofw_Suspectp) {
    gofw_Writep0(p);

  } else if (p > 1.0 - gofw_Suspectp) {
    if (p >= 1.0 - gofw_Epsilonp1) {
      printf(" 1 - eps1");
    } else if (p >= 1.0 - 1.0e-4) {
      printf(" 1 - ");
      num_WriteD(1.0 - p, 7, 2, 2);
      /* printf (" 1 - %.2g ", 1.0 - p); */
    } else if (p >= 1.0 - 1.0e-2)
      printf("  %.4f ", p);
    else
      printf("   %.2f", p);
  }
}

/*=========================================================================*/

static void
WriteReport(char *genName,        /* Generator or file name */
            char *batName,        /* Battery name */
            int N,                /* Max. number of tests */
            double pVal[],        /* p-values of the tests */
            chrono_Chrono *Timer, /* Timer */
            lebool Flag,          /* = TRUE for a file, FALSE for a gen */
            lebool VersionFlag,   /* = TRUE: write the version number */
            double nb             /* Number of bits in the random file */
) {
  int j, co;

  printf("\n========= Summary results of ");
  printf("%s", batName);
  printf(" =========\n\n");
  if (VersionFlag)
    printf(" Version:          %s\n", PACKAGE_STRING);
  if (Flag)
    printf(" File:             ");
  else
    printf(" Generator:        ");
  printf("%s", genName);
  if (nb > 0)
    printf("\n Number of bits:   %.0f", nb);
  co = 0;
  /* Some of the tests have not been done: their pVal[j] < 0. */
  for (j = 0; j < N; j++) {
    if (pVal[j] >= 0.0)
      co++;
  }
  printf("\n Number of statistics:  %1d\n", co);
  printf(" Total CPU time:   ");
  chrono_Write(Timer, chrono_hms);

  co = 0;
  for (j = 0; j < N; j++) {
    if (pVal[j] < 0.0) /* That test was not done: pVal = -1 */
      continue;
    if ((pVal[j] < gofw_Suspectp) || (pVal[j] > 1.0 - gofw_Suspectp)) {
      co++;
      break;
    }
  }
  if (co == 0) {
    printf("\n\n All tests were passed\n\n\n\n");
    return;
  }

  if (gofw_Suspectp >= 0.01)
    printf("\n The following tests gave p-values outside [%.4g, %.2f]",
           gofw_Suspectp, 1.0 - gofw_Suspectp);
  else if (gofw_Suspectp >= 0.0001)
    printf("\n The following tests gave p-values outside [%.4g, %.4f]",
           gofw_Suspectp, 1.0 - gofw_Suspectp);
  else if (gofw_Suspectp >= 0.000001)
    printf("\n The following tests gave p-values outside [%.4g, %.6f]",
           gofw_Suspectp, 1.0 - gofw_Suspectp);
  else
    printf("\n The following tests gave p-values outside [%.4g, %.14f]",
           gofw_Suspectp, 1.0 - gofw_Suspectp);
  printf(":\n (eps  means a value < %6.1e)", gofw_Epsilonp);
  printf(":\n (eps1 means a value < %6.1e)", gofw_Epsilonp1);
  printf(":\n\n       Test                          p-value\n");
  printf(" ----------------------------------------------\n");

  co = 0;
  for (j = 0; j < N; j++) {
    if (pVal[j] < 0.0) /* That test was not done: pVal = -1 */
      continue;
    if ((pVal[j] >= gofw_Suspectp) && (pVal[j] <= 1.0 - gofw_Suspectp))
      continue; /* That test was passed */
    printf(" %2d ", TestNumber[j]);
    printf(" %-30s", bbattery_TestNames[j]);
    WritepVal(pVal[j]);
    printf("\n");
    co++;
  }

  printf(" ----------------------------------------------\n");
  if (co < N - 1) {
    printf(" All other tests were passed\n");
  }
  printf("\n\n\n");
}

/*=========================================================================*/

static void GetPVal_Walk(long N, swalk_Res *res, int *pj, char *mess, int j2)
/*
 * Get the p-values in a swalk_RandomWalk1 test
 */
{
  int j = *pj;
  const unsigned int len = 20;

  if (N == 1) {
    bbattery_pVal[++j] = res->H[0]->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 H");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->M[0]->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 M");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->J[0]->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 J");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->R[0]->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 R");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->C[0]->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 C");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

  } else {
    bbattery_pVal[++j] = res->H[0]->pVal2[gofw_Sum];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 H");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->M[0]->pVal2[gofw_Sum];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 M");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->J[0]->pVal2[gofw_Sum];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 J");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->R[0]->pVal2[gofw_Sum];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 R");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);

    bbattery_pVal[++j] = res->C[0]->pVal2[gofw_Sum];
    TestNumber[j] = j2;
    strcpy(CharTemp, "RandomWalk1 C");
    strncat(CharTemp, mess, (size_t)len);
    strncpy(bbattery_TestNames[j], CharTemp, (size_t)LEN);
  }

  *pj = j;
}

/*=========================================================================*/

static void GetPVal_CPairs(long N, snpair_Res *res, int *pj, char *mess, int j2)
/*
 * Get the p-values in a snpair_ClosePairs test
 */
{
  int j = *pj;
  const unsigned int len = 20;

  if (N == 1) {
    bbattery_pVal[++j] = res->pVal[snpair_NP];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs NP");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    bbattery_pVal[++j] = res->pVal[snpair_mNP];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs mNP");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

  } else {
    bbattery_pVal[++j] = res->pVal[snpair_NP];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs NP");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    bbattery_pVal[++j] = res->pVal[snpair_mNP];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs mNP");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    bbattery_pVal[++j] = res->pVal[snpair_mNP1];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs mNP1");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    bbattery_pVal[++j] = res->pVal[snpair_mNP2];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs mNP2");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    bbattery_pVal[++j] = res->pVal[snpair_NJumps];
    TestNumber[j] = j2;
    strcpy(CharTemp, "ClosePairs NJumps");
    strncat(CharTemp, mess, (size_t)len);
    strcpy(bbattery_TestNames[j], CharTemp);

    if (snpair_mNP2S_Flag) {
      bbattery_pVal[++j] = res->pVal[snpair_mNP2S];
      TestNumber[j] = j2;
      strcpy(CharTemp, "ClosePairs mNP2S");
      strncat(CharTemp, mess, (size_t)len);
      strcpy(bbattery_TestNames[j], CharTemp);
    }
  }

  *pj = j;
}

/*=========================================================================*/

static void InitBat(void)
/*
 * Initializes the battery of tests: sets all p-values to -1.
 */
{
  int j;
  static int flag = 0;
  for (j = 0; j < NDIM; j++)
    bbattery_pVal[j] = -1.0;
  if (0 == flag) {
    flag++;
    for (j = 0; j < NDIM; j++)
      bbattery_TestNames[j] = util_Calloc(LEN + 1, sizeof(char));
  }
}

/*=========================================================================*/

static void SmallCrush(unif01_Gen *gen, char *filename, int Rep[])
/*
 * A small battery of statistical tests for Random Number Generators
 * used in simulation.
 * Rep[i] gives the number of times that test i will be done. The default
 * values are Rep[i] = 1 for all i.
 */
{
  const int r = 0;
  int i;
  int j = -1;
  int j2 = 0;
  char genName[LEN + 1] = "";
  chrono_Chrono *Timer;
  sres_Poisson *res1;
  sres_Chi2 *res2;
  sknuth_Res2 *res3;
  swalk_Res *res4;
  sknuth_Res1 *res5;
  sstring_Res *res6;
  lebool fileFlag;

  Timer = chrono_Create();
  InitBat();
  if (swrite_Basic) {
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
           "                 Starting SmallCrush\n"
           "                 Version: %s\n"
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
           PACKAGE_STRING);
  }

  if (NULL == gen) {
    gen = ufile_CreateReadText(filename, 10 * MILLION);
    fileFlag = TRUE;
  } else
    fileFlag = FALSE;

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  res1 = sres_CreatePoisson();
  for (i = 0; i < Rep[j2]; ++i) {
#ifdef USE_LONGLONG
    smarsa_BirthdaySpacings(gen, res1, 1, 5 * MILLION, r, 1073741824, 2, 1);
#else
    smarsa_BirthdaySpacings(gen, res1, 10, MILLION / 2, r, 67108864, 2, 1);
#endif
    bbattery_pVal[++j] = res1->pVal2;
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "BirthdaySpacings");
  }
  sres_DeletePoisson(res1);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res3 = sknuth_CreateRes2();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_Collision(gen, res3, 1, 5 * MILLION, 0, 65536, 2);
    bbattery_pVal[++j] = res3->Pois->pVal2;
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "Collision");
  }
  sknuth_DeleteRes2(res3);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res2 = sres_CreateChi2();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_Gap(gen, res2, 1, MILLION / 5, 22, 0.0, .00390625);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "Gap");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_SimpPoker(gen, res2, 1, 2 * MILLION / 5, 24, 64, 64);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "SimpPoker");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_CouponCollector(gen, res2, 1, MILLION / 2, 26, 16);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "CouponCollector");
  }

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res5 = sknuth_CreateRes1();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_MaxOft(gen, res5, 1, 2 * MILLION, 0, MILLION / 10, 6);
    bbattery_pVal[++j] = res5->Chi->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MaxOft");
    bbattery_pVal[++j] = res5->Bas->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MaxOft AD");
  }
  sknuth_DeleteRes1(res5);

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    svaria_WeightDistrib(gen, res2, 1, MILLION / 5, 27, 256, 0.0, 0.125);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "WeightDistrib");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    smarsa_MatrixRank(gen, res2, 1, 20 * THOUSAND, 20, 10, 60, 60);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MatrixRank");
  }
  sres_DeleteChi2(res2);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res6 = sstring_CreateRes();
  for (i = 0; i < Rep[j2]; ++i) {
    sstring_HammingIndep(gen, res6, 1, MILLION / 2, 20, 10, 300, 0);
    bbattery_pVal[++j] = res6->Bas->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "HammingIndep");
  }
  sstring_DeleteRes(res6);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  util_Assert(j2 <= SMALLCRUSH_NUM, "SmallCrush:   j2 > SMALLCRUSH_NUM");
  res4 = swalk_CreateRes();
  for (i = 0; i < Rep[j2]; ++i) {
    swalk_RandomWalk1(gen, res4, 1, MILLION, r, 30, 150, 150);
    GetPVal_Walk(1, res4, &j, "", j2);
  }
  swalk_DeleteRes(res4);

  bbattery_NTests = ++j;
  if (fileFlag) {
    WriteReport(filename, "SmallCrush", bbattery_NTests, bbattery_pVal, Timer,
                TRUE, TRUE, 0.0);
    ufile_DeleteReadBin(gen);
  } else {
    GetName(gen, genName);
    WriteReport(genName, "SmallCrush", bbattery_NTests, bbattery_pVal, Timer,
                FALSE, TRUE, 0.0);
  }
  chrono_Delete(Timer);
}

static void MidCrush(unif01_Gen *gen, char *filename, int Rep[])
/*
 * A mid battery of statistical tests for Random Number Generators
 * used in simulation.
 * Rep[i] gives the number of times that test i will be done. The default
 * values are Rep[i] = 1 for all i.
 */
{
  const int r = 0;
  int i;
  int j = -1;
  int j2 = 0;
  char genName[LEN + 1] = "";
  chrono_Chrono *Timer;
  sres_Poisson *res1;
  sres_Chi2 *res2;
  sknuth_Res2 *res3;
  swalk_Res *res4;
  sknuth_Res1 *res5;
  sstring_Res *res6;
  lebool fileFlag;

  Timer = chrono_Create();
  InitBat();
  if (swrite_Basic) {
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
           "                 Starting MidCrush\n"
           "                 Version: %s\n"
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
           PACKAGE_STRING);
  }

  if (NULL == gen) {
    gen = ufile_CreateReadText(filename, 10 * MILLION);
    fileFlag = TRUE;
  } else
    fileFlag = FALSE;

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  res1 = sres_CreatePoisson();
  for (i = 0; i < Rep[j2]; ++i) {
#ifdef USE_LONGLONG
    smarsa_BirthdaySpacings(gen, res1, 1, 5 * MILLION, r, 1073741824, 2, 1);
#else
    smarsa_BirthdaySpacings(gen, res1, 10, MILLION / 2, r, 67108864, 2, 1);
#endif
    bbattery_pVal[++j] = res1->pVal2;
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "BirthdaySpacings");
  }
  sres_DeletePoisson(res1);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res3 = sknuth_CreateRes2();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_Collision(gen, res3, 1, 5 * MILLION, 0, 65536, 2);
    bbattery_pVal[++j] = res3->Pois->pVal2;
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "Collision");
  }
  sknuth_DeleteRes2(res3);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res2 = sres_CreateChi2();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_Gap(gen, res2, 1, MILLION / 5, 22, 0.0, .00390625);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "Gap");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_SimpPoker(gen, res2, 1, 2 * MILLION / 5, 24, 64, 64);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "SimpPoker");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_CouponCollector(gen, res2, 1, MILLION / 2, 26, 16);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "CouponCollector");
  }

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res5 = sknuth_CreateRes1();
  for (i = 0; i < Rep[j2]; ++i) {
    sknuth_MaxOft(gen, res5, 1, 2 * MILLION, 0, MILLION / 10, 6);
    bbattery_pVal[++j] = res5->Chi->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MaxOft");
    bbattery_pVal[++j] = res5->Bas->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MaxOft AD");
  }
  sknuth_DeleteRes1(res5);

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    svaria_WeightDistrib(gen, res2, 1, MILLION / 5, 27, 256, 0.0, 0.125);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "WeightDistrib");
  }

  ++j2;
  if (fileFlag)
    ufile_InitReadText();
  for (i = 0; i < Rep[j2]; ++i) {
    smarsa_MatrixRank(gen, res2, 1, 20 * THOUSAND, 20, 10, 60, 60);
    bbattery_pVal[++j] = res2->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "MatrixRank");
  }
  sres_DeleteChi2(res2);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  res6 = sstring_CreateRes();
  for (i = 0; i < Rep[j2]; ++i) {
    sstring_HammingIndep(gen, res6, 1, MILLION / 2, 20, 10, 300, 0);
    bbattery_pVal[++j] = res6->Bas->pVal2[gofw_Mean];
    TestNumber[j] = j2;
    strcpy(bbattery_TestNames[j], "HammingIndep");
  }
  sstring_DeleteRes(res6);

  if (fileFlag)
    ufile_InitReadText();
  ++j2;
  util_Assert(j2 <= MIDCRUSH_NUM, "MidCrush:   j2 > MIDCRUSH_NUM");
  res4 = swalk_CreateRes();
  for (i = 0; i < Rep[j2]; ++i) {
    swalk_RandomWalk1(gen, res4, 1, MILLION, r, 30, 150, 150);
    GetPVal_Walk(1, res4, &j, "", j2);
  }
  swalk_DeleteRes(res4);
#if 1

  {
    const int s = 30;
    sres_Chi2 *res;
    res = sres_CreateChi2();
    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sstring_PeriodsInStrings(gen, res, 1, 300 * MILLION, r, s);
      bbattery_pVal[++j] = res->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "PeriodsInStrings, r = 0");
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sstring_PeriodsInStrings(gen, res, 1, 300 * MILLION, 15, 15);
      bbattery_pVal[++j] = res->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "PeriodsInStrings, r = 15");
    }
    sres_DeleteChi2(res);
  }
  {
    const int s = 30;
    sres_Basic *res;
    res = sres_CreateBasic();
    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sstring_HammingWeight2(gen, res, 100, 100 * MILLION, r, s, MILLION);
      bbattery_pVal[++j] = res->pVal2[gofw_Sum];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "HammingWeight2, r = 0");
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sstring_HammingWeight2(gen, res, 30, 100 * MILLION, 20, 10, MILLION);
      bbattery_pVal[++j] = res->pVal2[gofw_Sum];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "HammingWeight2, r = 20");
    }
    sres_DeleteBasic(res);
  }

  {
    const int s = 30;

    sspectral_Res *res;
    res = sspectral_CreateRes();

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sspectral_Fourier3(gen, res, 50 * THOUSAND, 14, r, s);
      bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "Fourier3, r = 0");
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      sspectral_Fourier3(gen, res, 50 * THOUSAND, 14, 20, 10);
      bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "Fourier3, r = 20");
    }
    sspectral_DeleteRes(res);
  }
  {
    scomp_Res *res;
    res = scomp_CreateRes();
    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      scomp_LinearComp(gen, res, 1, 120 * THOUSAND, r, 1);
      bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "LinearComp, r = 0");
      bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "LinearComp, r = 0");
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      scomp_LinearComp(gen, res, 1, 120 * THOUSAND, 29, 1);
      bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "LinearComp, r = 29");
      bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "LinearComp, r = 29");
    }
    scomp_DeleteRes(res);
  }

  {
    lebool flag = snpair_mNP2S_Flag;
    snpair_Res *res;
    res = snpair_CreateRes();

    snpair_mNP2S_Flag = FALSE;
    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      snpair_ClosePairs(gen, res, 10, 2 * MILLION, 0, 2, 0, 30);
      GetPVal_CPairs(10, res, &j, ", t = 2", j2);
    }

    snpair_mNP2S_Flag = TRUE;
    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      snpair_ClosePairs(gen, res, 10, 2 * MILLION, 0, 3, 0, 30);
      GetPVal_CPairs(10, res, &j, ", t = 3", j2);
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      snpair_ClosePairs(gen, res, 5, 2 * MILLION, 0, 7, 0, 30);
      GetPVal_CPairs(10, res, &j, ", t = 7", j2);
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      snpair_ClosePairsBitMatch(gen, res, 4, 4 * MILLION, 0, 2);
      bbattery_pVal[++j] = res->pVal[snpair_BM];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "ClosePairsBitMatch, t = 2");
    }

    ++j2;
    for (i = 0; i < Rep[j2]; ++i) {
      snpair_ClosePairsBitMatch(gen, res, 2, 4 * MILLION, 0, 4);
      bbattery_pVal[++j] = res->pVal[snpair_BM];
      TestNumber[j] = j2;
      strcpy(bbattery_TestNames[j], "ClosePairsBitMatch, t = 4");
    }
    snpair_DeleteRes(res);
    snpair_mNP2S_Flag = flag;
  }

#endif
  bbattery_NTests = ++j;
  if (fileFlag) {
    WriteReport(filename, "MidCrush", bbattery_NTests, bbattery_pVal, Timer,
                TRUE, TRUE, 0.0);
    ufile_DeleteReadBin(gen);
  } else {
    GetName(gen, genName);
    WriteReport(genName, "MidCrush", bbattery_NTests, bbattery_pVal, Timer,
                FALSE, TRUE, 0.0);
  }
  chrono_Delete(Timer);
}

/*=========================================================================*/

void mid_crush(unif01_Gen *gen) {
  int i;
  int Rep[1 + NDIM] = {0};
  Rep[1] = 1;
  // for (i = 1; i <= MIDCRUSH_NUM; ++i) {
  //   Rep[i] = 1;
  // }
  MidCrush(gen, NULL, Rep);
}

/*=========================================================================*/
