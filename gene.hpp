//
//  gene.hpp
//  huawei3
//
//  Created by sanshanxiashi on 2017/3/24.
//  Copyright © 2017年 sanshanxiashi. All rights reserved.
//

#ifndef gene_hpp
#define gene_hpp

#include <stdio.h>
#include<vector>
#include<ctime>
using namespace std;
#endif /* gene_hpp */

double rand01();

double fitNessFun(double x ,vector<int>& choiceNode);

int GA(int cmin[1005][505] , struct MCMF& huaReal ,vector<int> choiceNode , int pop[45][1505] , int best_individual[1505], clock_t start);
