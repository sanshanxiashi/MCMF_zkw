//
//  gene.cpp
//  huawei3
//
//  Created by sanshanxiashi on 2017/3/24.
//  Copyright © 2017年 sanshanxiashi. All rights reserved.
//

#include "gene.hpp"
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <cstdio>
#include <iostream>
#include "maxFlow.hpp"
#define  C 2000

using namespace std;
int N,M;


struct FitTime{
    int x,indx;
};


bool cmpFit(FitTime a, FitTime b){
    return a.x > b.x ;
}

bool cmpFitUp(FitTime a, FitTime b){
    return a.x < b.x ;
}



//产生（0，1）均匀分布的随机数
double rand01(){
    double x;
    x = (double)rand()/RAND_MAX;
    return x;
}


double fitNessFun2(int p[1505], struct MCMF hua, vector<int>& choiceNode){ //p[]表示一个染色体
    int ret;
    
    //判断当前解的 最小费用流
    string str;
    
    vector<int> serverList;
    for(int i=0; i<M ; i++)
        if(p[i]) serverList.push_back(choiceNode[i]) ;
    
    int serverNum = (int)serverList.size() ;
    /*   for(int i=0; i< (int)serverList.size(); i++)
     printf("%d ",serverList[i]);
     puts("");
     */
    ret = getServersAndGetMincost(serverList, serverNum , hua, hua, str ,0 );
    
    return ret;
}




//利用费用函数cmin[i][j], 确定serverList
int GA(int cmin[1005][505], struct MCMF& huaReal , vector<int> choiceNode, int pop[45][1505] , int best_individual[1505] , clock_t start){
    
    int ret;
    int _best_cost = INF;
    srand( (unsigned)time(NULL) );//初始化时间种子
    
    N=41;//群体大小，指群体包含的个体数目，一般取(20-100)
    M=(int)choiceNode.size() ;//染色体的码元数
    int Generation=1000;//遗传算法的终止迭代代数，一般取(100-500)
    //  double prob_cross=0.7;//交叉概率，一般取(0.4-0.99) ,0.7
    //  double prob_mutation=0.1;//变异概率，一般取(0.0001,0.1) ,0.1
    // int pop[45][1505] = {0};//种群变量
    // best_individual[1505];//保存一代中的最优个体
    
    double p_c0 = 0.4, p_c1= 0.6, p_c2= 0.9 , p_c;
    double p_m0 = 0.0004 , p_m1= 0.001, p_m2= 0.3 , p_m;
    double afa= 0.78 , beta = 0.0005;
    
    
    //初始化第一个个体为 ： 直连点全连的情况
    for(int i=0; i< M; i++)
        best_individual[i]= pop[0][i] ;
    
    
    //----------------------
    //遗传算法迭代核心
    int iGeneration = Generation;
    
    double fit[45]; //群体的适应度
    double fit_best =INF ;
    double fit_max , fit_aver, fit_min ,fit_sum;
    int best_indx = -1;
    double fit_total=0;    //总适应度
    double fit_temp[45]; //记录累加的适应度
    FitTime fitness[45];
    
    
    
    while( iGeneration--){
        
        
        printf("\niGeneration:=%d\n",iGeneration);
        //选择。计算群体中每个个体的适应度
        memset(fit, 0 ,sizeof(fit));
        
        best_indx = 0;
        fit_max = 0;
        for(int i=0; i< N; i++){
            // p = pop[i];
            // printf("%2d:",i);
            fit[i] = fitNessFun2(pop[i] , huaReal ,choiceNode);
            if(fit[i]>0 && fit[i]<fit_best) {
                fit_best=fit[i];
                best_indx = i;
            }
            if(fit[i] > fit_max) fit_max = fit[i];
            
             //   printf("fit_cost =%.3lf   , best_cost = %.3lf  best_indx =%d \n", fit[i] , fit_best ,best_indx);
        }
        
        
        
        //重新定义 评价函数
        for(int i=0; i<N; i++){
            fitness[i].x = (fit[i]>0)? (fit_max - fit[i]+10): 1;
            fitness[i].indx = i;
        }
        
        
        //保留品质最好的一个个体，直接进入下一轮遗传
        for(int i=0; i<M ; i++)
            pop[0][i]= best_individual[i] = pop[best_indx][i];
        
        fit_total=0;
        for(int i=0 ; i< N; i++)
        {
            fit_total += fitness[i].x;
            fit_temp[i] = fit_total;
        }
        
        int tmp_pop[45][1505];
        //选择算子
        for(int i=1 ; i< N; i++){
            
            //轮盘赌算法
            int rand_tmp = rand01() * (int)fit_total;
            // printf("i= %d , rand =%d\n", i, rand_tmp);
            for(int j=0; j<N ; j++)
                if(rand_tmp < fit_temp[j]){
                    for(int k=0; k<M ; k++)
                        tmp_pop[i][k] = pop[j][k];
                    break;
                }
            
            /*
             //锦标赛选择法
             int rand_t1 = (int)(rand01() *N) , rand_t2 = (int)(rand01() *N);
             int big = fitness[rand_t1].x > fitness[rand_t2].x? rand_t1: rand_t2;
             for(int k=0; k<M ; k++)
             tmp_pop[i][k] = pop[big][k];
             printf("%d: %d ,  %d: %d , bigger = %d\n",rand_t1, fitness[rand_t1].x , rand_t2 , fitness[rand_t2].x ,big);
             */
        }
        
        
        
        for(int i=1; i< N ; i++)  //选择后组成的群体 赋给初始群体
            for(int j=0; j<M; j++)
                pop[i][j] = tmp_pop[i][j];
        
        
        //输出所有 个体
        /*        for(int i=0; i< N; i++){
         printf("%4d:  ", i);
         for(int j=0; j<M; j++)
         printf("%d",pop[i][j]);
         puts("");
         }
         
         printf("En\n\n\n\n");
         */
        //确定 基因交叉概率p_c和 变异概率p_m
        
        fit_min = fitness[0].x;
        fit_max = fitness[0].x;
        fit_sum = 0;
        for(int i=0; i<N; i++){
            fit_min = (fitness[i].x < fit_min)? fitness[i].x: fit_min;
            fit_max = (fitness[i].x > fit_min)? fitness[i].x: fit_max;
            fit_sum += fitness[i].x;
        }
        fit_aver = fit_sum/N;
        
        //p_c
        if(fit_aver/fit_max > afa){
            if(fit_min/fit_max >= p_c1/p_c2)
                p_c = p_c0;
            else if(fit_min/fit_max > beta && fit_min/fit_max < p_c1/p_c2)
                p_c = p_c2 - (p_c2-p_c1)/(1-fit_min/fit_max);
            else p_c = p_c2;
        }else p_c = p_c2 ;
        
        
        //p_m
        if(fit_aver/fit_max > afa){
            if(fit_min/fit_max >= p_m1/p_m2)
                p_m = p_m2;
            else if(fit_min/fit_max > beta && fit_min/fit_max < p_m1/p_m2)
                p_m = p_m2 - (p_m2-p_m1)/(1-fit_min/fit_max);
            else p_m = p_m0;
        }else p_m = p_m0;
        
      //  printf("p_c =%.6lf , p_m =%.6lf\n", p_c , p_m);
        //----------------
        //基因交换 单点
        /*
         for(int i=1; i< N ; i+=2){
         int cross_indx= -1;
         
         if(rand01() < p_c){  //以一定概率 交换基因
         cross_indx = int(M* rand01());
         }
         
         if(cross_indx>0 && cross_indx<M){
         for(int j=cross_indx ; j<M ; j++) {
         int a= fitness[i].indx , b= fitness[i+1].indx ;
         int t = pop[a][j];
         pop[a][j] = pop[b][j];
         pop[b][j] = t;
         }
         }
         }
         */
        // 基因交换 均匀
        //随机排序
        random_shuffle(fitness+1, fitness+N);
        for(int i=1; i<N; i+=2){
            double p_k = rand01();
            
            if(p_k < p_c){
                int a = fitness[i].indx , b = fitness[i+1].indx;
                int tmplate[1505]; //模板个体
                memset(tmplate, 0 ,sizeof(tmplate));
                int oneCnt = (int)(0.3*M); //模板中1的个数
                while(oneCnt){
                    int indx = rand01()*M;
                    if(!tmplate[indx]) {
                        tmplate[indx] = 1;
                        oneCnt --;
                    }
                }
                
                //交换
                for(int j=0 ; j<M; j++)
                    if(tmplate[j]){ int t = pop[a][j]; pop[a][j] = pop[b][j]; pop[b][j] = t; }
            }
            
        }
        
        
        
        
        //----------------
        //基因变异
        for(int i=1; i< N; i++){
            if( rand01()< p_m){
                int mutation_indx = int(M*rand01());
                int pos = fitness[i].indx;
                pop[pos][mutation_indx]= pop[pos][mutation_indx]? 0:1;
            }
        }
        
        
        if( (clock()-start >80*CLOCKS_PER_SEC))
        {
            printf("end during time =%d\n",(int)(clock() - start));
            break;
        }
       // printf("myGod\n");
    }
    
    
    
    return ret;
    
}
