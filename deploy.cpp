#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <string.h>
#include "maxFlow.hpp"
#include "gene.hpp"

using namespace std;

bool cmpUpNode(struct Node a, struct Node b){
    return a.f > b.f;
}


void data_init(char * topo[MAX_EDGE_NUM] , struct MCMF& huawei){
    
    int netNode,cnt,consumeNode, serverCost;
    int s,t,k;
    int from, to , cap , cost;
    int topoCnt=0; //用来读topo中的每行
    
    
    
    
    //网络节点数量 网络链路数量 消费节点数量
    sscanf(topo[topoCnt++] ,"%d%d%d",&netNode, &cnt , &consumeNode);
    topoCnt++;
    sscanf(topo[topoCnt++],"%d",&serverCost);
    topoCnt++;
    //printf("serverCost= %d\n", serverCost);
    
    huawei.init(netNode+consumeNode+2,netNode,consumeNode,serverCost);
    
    //初始化m个网络链路
    while(cnt--){
        sscanf(topo[topoCnt++], "%d%d%d%d",&from , &to, &cap, &cost);
        //printf("cnt=%d : from =%d  to=%d  cap=%d  cost=%d \n",cnt, from, to ,cap, cost);
        huawei.AddEdge(from, to, cap, cost);
        huawei.AddEdge(to,from,cap,cost);
        
    }
    topoCnt++;
    
    //源点、汇点
    s=netNode+consumeNode;
    t=s+1;
    
    //记录汇点的总流量k
    k=0;
    huawei.consumeMaxFlow = 0;
    //初始化 消费节点位置
    for(int i=0; i<consumeNode; i++){
        int indx,nodeID,cap;
        sscanf(topo[topoCnt++], "%d%d%d",&indx, &nodeID, &cap);
        k+= cap;
        
        //记录最大的 消费点容量
        if(huawei.consumeMaxFlow < cap) huawei.consumeMaxFlow = cap;
        
        //消费点与网络节点相连
        huawei.AddEdge(indx+netNode , nodeID, cap, 0);
        //与消费点相连的边的编号
        int m = huawei.size;
        huawei.consumeEdge[indx] = m-2;
        
        huawei.AddEdge(nodeID, indx+netNode , cap, 0);
        
        //消费点与汇点相连
        huawei.AddEdge(indx+netNode, t, cap, 0);
        
        //标记某个网络点是否相连 某个消费点
        huawei.isNetToCons[nodeID] = indx ;
        huawei.conflowNeed[indx] = cap;
    }
    huawei.k =k;
    
}



void makeSolutionsForGA(int chrome[45][1505],struct MCMF hua){
    
    string str;
    vector<int>serverList;
    vector<int>consumeList;
    
    for(int i=0; i<41; i++)
        memset(chrome[i] , 0 ,sizeof(chrome[i]));
    
    //消费点
    for(int i=0; i<hua.consumeNode; i++)
        consumeList.push_back(i);
    
    int stardFlow = 0.2 * hua.consumeMaxFlow;
    int chromeCnt=0;
    int iter=0;
    int times = 41;
    if(hua.netNode == 800) stardFlow = 60 , times = 21;
    //正向
    while(iter++ < times){
        
        serverList.clear();
        int nowCap = stardFlow + iter;
        int cntBig = 0;
        printf("\nchromeCnt= %d ,  nowCap =%d\n", chromeCnt, nowCap);
        for(int i=0; i<hua.consumeNode; i++){
            
            EDGE& e = hua.E[hua.consumeEdge[i]];
            if(e.cap >= nowCap)
            {
                serverList.push_back(e.v );
                chrome[chromeCnt][i] = 1;
                cntBig ++;
            }
        }
       // printf("大于 %d 的容量有 %d 个。\n", nowCap, cntBig);
        
        struct MCMF huaTmp = hua;
        int serverNum = (int)serverList.size();
        int ret = newGetServersAndGetMincost2(serverList, consumeList,  serverNum , huaTmp, str);
        printf("ret=%d\n",ret);
        
     
        //补全
        for(int i=0; i<huaTmp.consumeNode; i++){
           //  printf("%d : flow =%d  , flowNeed =%d\n",i,  huaTmp.conflow[i], huaTmp.conflowNeed[i]);
            if(huaTmp.conflow[i]<huaTmp.conflowNeed[i])
            {
                chrome[chromeCnt][i] = 1;
                //serverList.push_back(huaTmp.edges[hua.consumeEdge[i]].to);
                serverList.push_back(huaTmp.E[hua.consumeEdge[i]].v);
             //   printf("add new server:%d  near consume :%d\n",huaTmp.E[hua.consumeEdge[i]].v, i);
            }
        }
       
        chromeCnt++;
        
        //end of while
    }
    
    printf("gou zao ok!\n");
    
}


//构造可行解
void modifiedPop(int pop[45][505], vector<int> sureServerList ,vector<int> choiceNode, struct MCMF hua , int times , map<int,int> choiceMap){
    
    vector<int >serverList;
    vector<int >consumeList;
    string str;
    
    //消费点
    for(int i=0; i<hua.consumeNode; i++)
        consumeList.push_back(i);
    
    
    for(int t = 0; t< times; t++){
    
        serverList.clear();
        for(int i=0; i<(int)sureServerList.size(); i++)
            serverList.push_back(sureServerList[i]);
        
        for(int i=0; i<(int)choiceNode.size(); i++)
            if(pop[t][i]) serverList.push_back(choiceNode[i]);
    
        struct MCMF huaTmp = hua;
        int serverNum = (int)serverList.size();
        int ret = newGetServersAndGetMincost2(serverList, consumeList,  serverNum , huaTmp, str);
        
        //补全
        for(int i=0; i<huaTmp.consumeNode; i++){
            // printf("%d : flow =%d  , flowNeed =%d\n",i,  huaTmp.conflow[i], huaTmp.conflowNeed[i]);
            if(huaTmp.conflow[i]<huaTmp.conflowNeed[i])
            {
                if(pop[t][choiceMap[i]]) {
                    puts("cao");
                    printf("!cap= %d \n",huaTmp.E[hua.consumeEdge[i]].cap);
                }
                pop[t][choiceMap[i]] = 1;
                serverList.push_back(huaTmp.E[hua.consumeEdge[i]].v);
                printf("add new server:%d  near consume :%d\n",huaTmp.E[hua.consumeEdge[i]].v, i);
            }
        }
    
    
    }


}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    
    int cmin[1005][505]; //每个网络点到消费点的最小花费
    int pop[45][1505];
    
    string str;
    vector<int> serverList;
    vector<int> choiceNode;
    vector<int> direct_nxt_Node;
    int best_individual[1505];
    
    
    //读入数据
    struct MCMF huawei , huawei0;
    huawei.costNow = -1;
    data_init(topo, huawei);
    huawei0 = huawei;
    
    
    // for_test(huawei0);
    
    clock_t start = clock();
    //先跑出来一个较优解
    //  serverList.clear();
    //  bestNear(serverList,  huawei0, str);
    
    //构造0-41的可行解
    makeSolutionsForGA(pop,huawei);
    
    if(huawei.netNode<=1000){
        
        //------------待GA选择的点
        
        // memset(pop[0], 0 ,sizeof(pop[0]));
        for(int i=0; i< huawei.consumeNode ; i++){
            //直连点
            int direct = huawei.E[huawei.consumeEdge[i]].v;
            choiceNode.push_back(direct);
            
        }
        
        
        printf("choice.size = %d\n", (int)choiceNode.size());
        
        printf("start time = %d ", (int)start);
        
        
        
        int ans = GA(cmin , huawei ,choiceNode , pop , best_individual ,start);
        
        
        
        
        serverList.clear();
        for(int i=0; i<(int)choiceNode.size() ; i++)
            if(best_individual[i]) serverList.push_back(choiceNode[i]) ;
        
        int serverNum = (int)serverList.size() ;
        
        int  ret = getServersAndGetMincost(serverList, serverNum , huawei, huawei, str ,1 );
        
        
        printf("finalCost =%d\n",ret);
    }else{
        
        huawei0.search_path();
        huawei0.OutputPath(str);
    }
    
    
    
    
    
    // 需要输出的内容
    char * topo_file = (char *)str.c_str();
    
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);
    
}
