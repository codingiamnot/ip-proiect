#include <bits/stdc++.h>
#include <graphics.h>
#include <winbgim.h>

using namespace std;

struct punct
{
    float x, y, z;

    punct() {}

    punct(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

int n, m;
vector<punct> puncte,puncte_ecran;
vector<pair<int, int>> muchii;

void citire(string &address)
{
    FILE *input = fopen(address.c_str(), "rb");

    fread(&n, 4, 1, input);
    fread(&m, 4, 1, input);

    for(int i=0; i<n; i++)
    {
        int x, y, z;

        fread(&x, 4, 1, input);
        fread(&y, 4, 1, input);
        fread(&z, 4, 1, input);

        puncte.push_back(punct(x, y, z));
    }

    for(int i=0; i<m; i++)
    {
        int x, y;

        fread(&x, 4, 1, input);
        fread(&y, 4, 1, input);

        muchii.push_back({x, y});
    }

    fclose(input);
}

void salvare(string &address)
{
    FILE *output = fopen(address.c_str(), "wb");

    fwrite(&n, 4, 1, output);
    fwrite(&m, 4, 1, output);

    for(int i=0; i<n; i++)
    {
        fwrite(&puncte[i].x, 4, 1, output);
        fwrite(&puncte[i].y, 4, 1, output);
        fwrite(&puncte[i].z, 4, 1, output);
    }

    for(int i=0; i<m; i++)
    {
        fwrite(&muchii[i].first, 4, 1, output);
        fwrite(&muchii[i].second, 4, 1, output);
    }

    fclose(output);
}

punct centruGreutate()
{
    ///aprox: media aritmetica

    float sumX, sumY, sumZ;

    for(int i=0; i<n; i++)
    {
        sumX += puncte[i].x;
        sumY += puncte[i].y;
        sumZ += puncte[i].z;
    }

    sumX /= (double)n;
    sumY /= (double)n;
    sumZ /= (double)n;

    return punct(sumX, sumY, sumZ);
}

void translateaza(float dx, float dy, float dz)
{
    for(int i=0; i<n; i++)
    {
        puncte[i].x += dx;
        puncte[i].y += dy;
        puncte[i].z += dz;
    }
}

void rotestex(float alfa)
{
    for(int i=0 ; i<n ; i++)
    {
        puncte[i].x=puncte[i].x;
        puncte[i].y=puncte[i].y*cos(alfa)-puncte[i].z*sin(alfa);
        puncte[i].z=puncte[i].y*sin(alfa)+puncte[i].z*cos(alfa);
    }
}

void rotestey(float alfa)
{
    for(int i=0 ; i<n ; i++)
    {
        puncte[i].x=puncte[i].x*cos(alfa)+puncte[i].z*sin(alfa);
        puncte[i].y=puncte[i].y;
        puncte[i].z=-puncte[i].x*sin(alfa)+puncte[i].z*cos(alfa);
    }
}

void roteste(float alfa,float beta)
{
    punct G=centruGreutate();
    translateaza(-G.x,-G.y,-G.z);
    rotestex(alfa);
    rotestey(beta);
    translateaza(G.x,G.y,G.z);
}

void zoom(float x)
{
    punct G=centruGreutate();
    translateaza(-G.x,-G.y,-G.z);
    for(int i=0 ; i<n ; i++)
    {
        puncte[i].x*=x;
        puncte[i].y*=x;
        puncte[i].z*=x;
    }
    translateaza(G.x*x,G.y*x,G.z*x);
}

void translateaza2(float dx, float dy, float dz)
{
    for(int i=0; i<n; i++)
    {
        puncte_ecran[i].x += dx;
        puncte_ecran[i].y += dy;
        puncte_ecran[i].z += dz;
    }
}


void deseneaza_inteligent(float d)
{
    puncte_ecran.clear();
    puncte_ecran.resize(puncte.size());


    zoom(500);
    translateaza(-300,-200,100);
    roteste(0,0);
    for(int i=0 ; i<n ; i++)
    {
        puncte_ecran[i].x=puncte[i].x*d/puncte[i].z;
        puncte_ecran[i].y=puncte[i].y*d/puncte[i].z;
    }

    translateaza2(100,100,0);
    initwindow(800,600);
    for(int i=0 ; i<m ; i++)
        line(puncte_ecran[muchii[i].first].x, puncte_ecran[muchii[i].first].y, puncte_ecran[muchii[i].second].x, puncte_ecran[muchii[i].second].y);

    getch();
    closegraph();
}

void deseneaza()
{
    initwindow(800,600);

    translateaza(2,2,0);
    zoom(125);
    roteste(0.23,0.1);
    for(int j=2 ; j<500 ; j++)
    {
        roteste(0.1,0.2);
        /// setcolor(WHITE);
        for(int i=0 ; i<m ; i++)
            line(puncte[muchii[i].first].x, puncte[muchii[i].first].y, puncte[muchii[i].second].x, puncte[muchii[i].second].y);
        roteste(0.3,0.2);
        /// setcolor(WHITE);
        for(int i=0 ; i<m ; i++)
            line(puncte[muchii[i].first].x, puncte[muchii[i].first].y, puncte[muchii[i].second].x, puncte[muchii[i].second].y);
        ///delay(5);
        ///setcolor(BLACK);

        //for(int i=0 ; i<m ; i++)
          //  line(puncte[muchii[i].first].x, puncte[muchii[i].first].y, puncte[muchii[i].second].x, puncte[muchii[i].second].y);
    }

    getch();
    closegraph();
}

void test()
{
    ifstream fin("in.txt");
    int x,y,z;
    fin>>n>>m;
    for(int i=1 ; i<=n ; i++)
    {
        fin>>x>>y>>z;
        puncte.push_back(punct(x, y, z));
    }

    for(int i=1 ; i<=m ; i++)
    {
        fin>>x>>y;
        muchii.push_back({x, y});
    }

    deseneaza_inteligent(10);
    //deseneaza();
}

int main()
{
    test();
    return 0;
}
