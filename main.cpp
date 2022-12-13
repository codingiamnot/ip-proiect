#include <bits/stdc++.h>
#include <graphics.h>
#include <winbgim.h>

using namespace std;

const int screenWidth = 600, screenHeight = 600;

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
vector<punct> puncte;
vector<pair<int, int>> muchii;

float procentDeOcupareEcran = 0.75;

float rotatieX = 0, rotatieY =0;



bool animOn = true;

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

void roteste(float alfa, float beta)
{
    /*
    punct G=centruGreutate();
    translateaza(-G.x,-G.y,-G.z);
    rotestex(alfa);
    rotestey(beta);
    translateaza(G.x,G.y,G.z);
    */

    rotatieX += alfa;
    rotatieY += beta;
}

void zoom(float dx)
{
    procentDeOcupareEcran += dx;
    procentDeOcupareEcran = max((float)0, procentDeOcupareEcran);
    procentDeOcupareEcran = min((float)1, procentDeOcupareEcran);
}


void deseneaza(float d = 10)
{
    ///calc rotatie

    vector<punct> puncteRotite;
    punct g = centruGreutate();

    cout<<rotatieX<<' '<<rotatieY<<endl;

    for(auto pct : puncte)
    {
        cout<<pct.x<<' ';

        float x = pct.x , y = pct.y, z = pct.z;

        ///translatam
        x -= g.x;
        y -= g.y;
        z -= g.z;

        cout<<x<<' ';

        float nx, ny, nz;

        ///rotim x
        nx = x;
        ny = y * cos(rotatieX) - z * sin(rotatieX);
        nz = y * sin(rotatieX) + z * cos(rotatieX);

        x = nx;
        y = ny;
        z = nz;

        cout<<x<<' ';


        ///rotim y
        nx = x * cos(rotatieY) + z * sin(rotatieY);
        ny = y;
        nz = (-x) * sin(rotatieY) + z * cos(rotatieY);

        x = nx;
        y = ny;
        z = nz;

        cout<<x<<' ';

        ///translatam
        x += g.x;
        y += g.y;
        z += g.z;

        cout<<x<<endl;
        //cout<<x<<' '<<y<<' '<<z<<endl;

        puncteRotite.push_back(punct(x, y, z));
    }


    ///calc plane geometric values
    vector<punct> punctePlan;

    for(auto pct : puncteRotite)
    {
        float x = (pct.x / pct.z) * d;
        float y = (pct.y / pct.z) * d;

        cout<<pct.x<<' '<<pct.y<<' '<<pct.z<<" becomes "<<x<<' '<<y<<endl;

        punctePlan.push_back( punct(x, y, 0) );
    }

    cout<<"\n\n";


    ///calc screen values
    vector< pair<int, int> > puncteEcran;

    float minX = 1e5, maxX = -1e5;
    float minY = 1e5, maxY = -1e5;

    for(auto pct : punctePlan)
    {
        minX = min(minX, pct.x);
        maxX = max(maxX, pct.x);

        minY = min(minY, pct.y);
        maxY = max(maxY, pct.y);
    }

    ///maybe TODO, screen independent

    for(auto pct : punctePlan)
    {
        int x = ( ((pct.x - minX) / (maxX - minX)) - 0.5 ) * (float)screenHeight * procentDeOcupareEcran + screenHeight/2;
        int y = ( ((pct.y - minY) / (maxY - minY)) - 0.5 ) * (float)screenWidth * procentDeOcupareEcran + screenWidth / 2;

        cout<<pct.x<<' '<<pct.y<<" becomes "<<x<<' '<<y<<endl;

        puncteEcran.push_back( {x, y} );
    }

    //initwindow(screenWidth, screenHeight);

    for(int i=0;i<(int)muchii.size();i++)
    {
        int id1 = muchii[i].first;
        int id2 = muchii[i].second;

        line( puncteEcran[ id1 ].first, puncteEcran[ id1 ].second, puncteEcran[ id2 ].first, puncteEcran[ id2 ].second );
    }

    //getch();
    //closegraph();
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

    initwindow(screenWidth, screenHeight);

    for(int i=1 ; i<=m ; i++)
    {
        fin>>x>>y;
        muchii.push_back({x, y});
    }

    roteste(0.35, 0.1);

    deseneaza();

    getch();
    closegraph();
}

void mainLoop()
{
    ///TODO
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

    initwindow(screenWidth, screenHeight);


    while(true)
    {
        if(animOn)
            roteste(0.1, 0.1);

        cleardevice();

        deseneaza();
        this_thread::sleep_for( chrono::milliseconds( 100 / 3 ) );
    }
}

int main()
{
    //test();
    mainLoop();
    return 0;
}
