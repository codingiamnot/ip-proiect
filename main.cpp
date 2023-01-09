#include <bits/stdc++.h>
#include <graphics.h>
#include <winbgim.h>
#include <ctime>

using namespace std;

const int screenWidth = 600, screenHeight = 600;
long double er = 1e-7;

struct punct
{
    float marime;
    float x, y, z;

    //punct() {}

    punct(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

int n, m;

vector<punct> puncte, puncteAnterioare;
vector< pair<int, int>> puncteEcran;
vector<pair<int, int>> muchii, muchiiAnterioare;

vector <long double> dy;

vector<bool> viz;

float procentDeOcupareEcran = 0.75;

float rotatieX = 0, rotatieY =0, rotatieZ = 0;

bool toggleAnimation = false, toggleRendering = false;

int errorScreen=0, mainScreeen;

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
    for(int i=0; i<puncte.size(); i++)
    {
        puncte[i].x += dx;
        puncte[i].y += dy;
        puncte[i].z += dz;
    }
}

void roteste(float alfa, float beta, float omega)
{
    rotatieX += alfa;
    rotatieY += beta;
    rotatieZ += omega;
}

bool compZ(pair<punct, int> a, pair<punct, int> b)
{
    return a.first.z < b.first.z;
}

float fArie(punct a, punct b, punct c)
{
    float ans = a.x * b.y + b.x * c.y + c.x * a.y;
    ans -= a.x * c.y + a.y * b.x + c.x * b.y;

    return ans;
}

bool isInside(punct a, vector<punct> v)
{
    float er=1e-5;
    if((int)v.size() <= 2)
        return false;

    //cout<<"func inside pt "<<a.x<<' '<<a.y<<' ';

    v.push_back(v[0]);


    for(int i=0; i<(int)v.size()-1; i++)
    {
        float arie = fArie(a, v[i], v[i+1]);

        if(arie <= er)
            return false;
    }

    return true;
}

bool compPanta(int i, int j)
{
    return dy[i] < dy[j];
}

void reorder(vector<punct> &v)
{
    /*
    cout<<"reorder got\n";

    for(auto it : v)
        cout<<it.x<<' '<<it.y<<'\n';

    cout<<"\n\n";
    */

    int minP = 0;
    for(int i=1; i<(int)v.size(); i++)
    {
        if(v[i].x < v[minP].x || (abs(v[i].x - v[minP].x) <= er && v[i].y < v[minP].y) )
            minP = i;
    }

    //cout<<minP<<'\n';

    dy.clear();
    for(int i=0; i<(int)v.size(); i++)
    {
        if(i == minP)
            dy.push_back(-1e10);

        else if(v[minP].x == v[i].x)
        {
            if(v[i].y > v[minP].y)
                dy.push_back(1e9);
            else
                dy.push_back(-1e9);
        }

        else
            dy.push_back( (v[i].y - v[minP].y) / (v[i].x - v[minP].x ) );
    }

    /*
    cout<<"pante\n";

    for(int i=0;i<(int)v.size(); i++)
        cout<<dy[i]<<' ';
    cout<<'\n';
    */

    vector<int> perm;
    for(int i=0; i<(int)v.size(); i++)
        perm.push_back(i);
    sort(perm.begin(), perm.end(), compPanta);

    /*
    cout<<"perm\n";

    for(int it : perm)
        cout<<it<<' ';
    cout<<'\n';
    */

    vector<punct> ans;

    for(int i=0; i<(int)v.size(); i++)
        ans.push_back(v[ perm[ i ] ]);


    v.clear();

    for(auto x : ans)
    {
        while((int)v.size() >= 2 && fArie(x, v[(int)v.size()-2], v.back()) < 0 )
            v.pop_back();

        v.push_back(x);
    }

    /*
    cout<<"new poly is\n";

    for(auto it : v)
    {
        cout<<it.x<<' '<<it.y<<' '<<'\n';
    }
    cout<<"\n\n";
    */
}


void calcViz(vector<punct> &puncte)
{
    /*
    cout<<"viz got\n";

    for(auto it : puncte)
        cout<<it.x<<' '<<it.y<<' '<<it.z<<'\n';

    cout<<"\n\n";
    */

    viz.resize((int)puncte.size());

    vector< pair<punct, int> > v;

    vector< punct > poligon;

    for(int i=0; i<(int)puncte.size(); i++)
        v.push_back( {puncte[i], i} );

    sort(v.begin(), v.end(), compZ);

    /*
    cout<<"order is\n";

    for(auto it : v)
        cout<<it.first.x<<' '<<it.first.y<<' '<<it.first.z<<' '<<' '<<it.second<<'\n';
    cout<<"\n\n";
    */

    //cout<<"beg alg\n\n";

    for(auto pct : v)
    {
        //cout<<"add point "<<pct.first.x<<' '<<pct.first.y<<' '<<pct.first.z<<' ';

        if(isInside(pct.first, poligon))
        {
            //cout<<"inside\n";
            viz[ pct.second ] = false;
        }


        else
        {
            //cout<<"outside\n";

            viz[ pct.second ] = true;
            poligon.push_back( pct.first );

            reorder(poligon);
        }
    }

    /*
    cout<<"viz output\n";

    for(int i=0; i<(int)v.size(); i++)
        cout<<puncte[i].x<<' '<<puncte[i].y<<' '<<puncte[i].z<<' '<<viz[i]<<'\n';;
    cout<<endl;
    */
}

void deseneaza(float d = 10)
{
    ///calc rotatie

    vector<punct> puncteRotite;
    punct g = centruGreutate();

    for(auto pct : puncte)
    {
        float x = pct.x, y = pct.y, z = pct.z;

        ///translatam
        x -= g.x;
        y -= g.y;
        z -= g.z;

        float nx, ny, nz;

        ///rotim x
        nx = x;
        ny = y * cos(rotatieX) - z * sin(rotatieX);
        nz = y * sin(rotatieX) + z * cos(rotatieX);

        x = nx;
        y = ny;
        z = nz;


        ///rotim y
        nx = x * cos(rotatieY) + z * sin(rotatieY);
        ny = y;
        nz = (-x) * sin(rotatieY) + z * cos(rotatieY);

        x = nx;
        y = ny;
        z = nz;

        ///rotim z
        nx = x * cos(rotatieZ) - y * sin(rotatieZ);
        ny = x * sin(rotatieZ) + y * cos(rotatieZ);
        nz = z;

        x = nx;
        y = ny;
        z = nz;

        ///translatam
        x += g.x;
        y += g.y;
        z += g.z;

        puncteRotite.push_back(punct(x, y, z));
        puncteRotite.back().marime=pct.marime;
        /*pct.x=x;
        pct.y=y;
        pct.z=z;*/
        //pct=punct(x,y,z);
    }

    //calcViz(puncte);

    for(int i=0 ; i<puncte.size() ; i++)
        puncte[i]=puncteRotite[i];

    ///calc plane geometric values
    vector<punct> punctePlan;

    for(auto pct : puncteRotite)
    {
        float x = (pct.x / pct.z) * d;
        float y = (pct.y / pct.z) * d;

        punctePlan.push_back( punct(x, y, pct.z) );
        punctePlan.back().marime=pct.marime;
    }

    ///calc screen values
    puncteEcran.clear();

    ///TODO maybe change back
    float minX = -5, maxX = 5;
    float minY = -5, maxY = 5;

    /*
    for(auto pct : punctePlan)
    {
        minX = min(minX, pct.x);
        maxX = max(maxX, pct.x);

        minY = min(minY, pct.y);
        maxY = max(maxY, pct.y);
    }
    */

    ///maybe TODO, screen independent

    for(auto pct : punctePlan)
    {
        int x = ( ((pct.x * pct.marime - minX) / (maxX - minX)) - 0.5 ) * (float)screenHeight * procentDeOcupareEcran + screenHeight/2;
        int y = ( ((pct.y * pct.marime- minY) / (maxY - minY)) - 0.5 ) * (float)screenWidth * procentDeOcupareEcran + screenWidth / 2;

        puncteEcran.push_back( {x, y} );
    }

    calcViz(punctePlan);

    int aux;
    for(auto pct : punctePlan)
        aux=pct.x;

    for(int i=0; i<(int)muchii.size(); i++)
    {
        int id1 = muchii[i].first;
        int id2 = muchii[i].second;

        if(toggleRendering && (!viz[id1] || !viz[id2]))
            continue;

        line( puncteEcran[ id1 ].first, puncteEcran[ id1 ].second, puncteEcran[ id2 ].first, puncteEcran[ id2 ].second );
    }

    for(auto pct : puncteEcran)
    {
        putpixel((int)pct.first, (int)pct.second, getcolor());
    }
}

struct button
{
    int x1,y1,x2,y2;
    string txt;

    button(int xA, int yA, int xB, int yB, const string &text)
    {
        txt = text;
        x1 = xA;
        x2 = xB;
        y1 = yA;
        y2 = yB;
    }

    void draw()
    {
        setcolor(BLACK);

        line(x1, y1, x2, y1);
        line(x2, y1, x2, y2);
        line(x2, y2, x1, y2);
        line(x1, y2, x1, y1);

        setfillstyle(SOLID_FILL, WHITE);
        floodfill(x1+1,y1+1,BLACK);

        if (!txt.empty())
        {
            settextstyle(3, HORIZ_DIR, 1);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            setbkcolor(WHITE);
            setcolor(BLACK);
            outtextxy((x1 + x2) / 2, (y1 + y2) / 2 + 5, (char *)txt.c_str());
        }
    }

    bool isPressed()
    {
        int x=mousex(), y=mousey();
        if(x>=x1 && x<=x2 && y>=y1 && y<=y2)
            return true;
        return false;
    }
};


void drawError(const string &text1, const string &text2)
{
    errorScreen = initwindow(300, 200, "", 400, 250, false, false);
    settextstyle(3, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setbkcolor(BLACK);
    setcolor(WHITE);
    outtextxy(150, 75, (char *)text1.c_str());
    outtextxy(150, 125, (char *)text2.c_str());

    setcurrentwindow(mainScreeen);
}

void closeError()
{
    setcurrentwindow(errorScreen);
    if(ismouseclick(WM_LBUTTONDOWN))
        clearmouseclick(WM_LBUTTONDOWN), closegraph(errorScreen), errorScreen=0;
    setcurrentwindow(mainScreeen);
}

void drawPoint()
{
    if(toggleAnimation)
    {
        drawError("You can't draw ","during animation!\n");
        return;
    }

    bool drawn=0;

    while(!drawn)
    {
        if(errorScreen)
            closeError();

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            float x=mousex(), y=mousey();

            if(x>=screenWidth && x<=screenWidth+200 && y>=0 && y<=screenHeight)
                drawError("You can't draw ","a point there!\n");
            else
            {
                drawn = 1;
                punct G=centruGreutate();

                x = ((x - screenHeight/2 ) / procentDeOcupareEcran / (float)screenHeight + 0.5 ) * 10 - 5;
                y = ((y - screenWidth/2 ) / procentDeOcupareEcran / (float)screenWidth + 0.5 ) * 10 - 5;

                x = x / 10 * G.z;
                y = y / 10 * G.z;

                bool aux=toggleRendering;
                toggleRendering=0;
                rotatieX=0;
                rotatieY=0;
                rotatieZ=0;
                setcolor(DARKGRAY);
                deseneaza();
                puncte.push_back(punct(x,y,G.z));
                puncte.back().marime = 1;

                toggleRendering=aux;
                setcolor(WHITE);
                deseneaza();

            }
        }

    }
}

int findpoint(float x, float y)  ///momentan
{
    int pct=-1, eroare=5;

    for(int i=0 ; i<puncteEcran.size() && pct==-1 ; i++)
        if(puncteEcran[i].first-eroare <= x && x <= puncteEcran[i].first+eroare && puncteEcran[i].second-eroare<=y && y<=puncteEcran[i].second+eroare)
            pct = i;
    return pct;
}

void addEdge(int a, int b)
{
    float mijx , mijy , mijz;
    mijx=(puncte[a].x + puncte[b].x) / 2;
    mijy=(puncte[a].y + puncte[b].y) / 2;
    mijz=(puncte[a].z + puncte[b].z) / 2;
    puncte.push_back(punct(mijx , mijy , mijz));
    puncte.back().marime=puncte[a].marime;

    muchii.push_back({a , (int)puncte.size()-1});
    muchii.push_back({(int)puncte.size()-1 , b});
    /*
    int cont=a;
    float eps;
    if(puncte[a].x!=puncte[b].x)
    {
        eps=(puncte[b].x - puncte[a].x) / 10;

        float dy,dz;
        dy=(puncte[a].y-puncte[b].y) / (puncte[a].x-puncte[b].x);
        dz=(puncte[a].z-puncte[b].z) / (puncte[a].x-puncte[b].x);

        for(float i=puncte[a].x+eps ; i<puncte[b].x ; i+=eps)
        {
            float y, z;
            y=puncte[a].y + (i - puncte[a].x ) * dy;
            z=puncte[a].z + (i - puncte[a].x ) * dz;
            puncte.push_back(punct(i,y,z));
            puncte.back().marime=puncte[a].marime;
            muchii.push_back({cont, (int)puncte.size()-1});
            cont=(int)puncte.size()-1;
        }
        muchii.push_back({cont, b});
    }
    else if(puncte[a].y!=puncte[b].y)
    {
        eps=(puncte[b].y - puncte[a].y) / 10;

        float dz;
        dz=(puncte[a].z-puncte[b].z) / (puncte[a].y-puncte[b].y);

        for(float i=puncte[a].y+eps ; i<puncte[b].y ; i+=eps)
        {
            float z;
            z=puncte[a].z + (i - puncte[a].y ) * dz;
            puncte.push_back(punct(puncte[a].x , i , z));
            puncte.back().marime=puncte[a].marime;
            muchii.push_back({cont, (int)puncte.size()-1});
            cont=(int)puncte.size()-1;
        }
        muchii.push_back({cont, b});
    }
    else
    {
        eps=(puncte[b].z - puncte[a].z) / 10;

        for(float i=puncte[a].z+eps ; i<puncte[b].z ; i+=eps)
        {
            puncte.push_back(punct(puncte[a].x , puncte[a].y , i));
            puncte.back().marime=puncte[a].marime;
            muchii.push_back({cont, (int)puncte.size()-1});
            cont=(int)puncte.size()-1;
        }
        muchii.push_back({cont, b});
    }*/
    //muchii.push_back({a,b});
}

void drawEdge()
{
    if(toggleAnimation)
    {
        drawError("You can't draw ","during animation!\n");
        return;
    }

    int gasit1 = -1, gasit2 = -1;

    while(gasit1 == -1 || gasit2 == -1)
    {
        if(errorScreen)
            closeError();

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            float x=mousex(), y=mousey();
            int pct=findpoint(x,y);

            if(pct == -1)
                drawError("There is no","point there!");
            else if(gasit1 == -1)
                gasit1 = pct;
            else gasit2 = pct;
        }
    }

    bool aux=toggleRendering;
    toggleRendering=0;
    rotatieX=0;
    rotatieY=0;
    rotatieZ=0;
    setcolor(DARKGRAY);
    deseneaza();
    addEdge(gasit1,gasit2);

    toggleRendering=aux;
    setcolor(WHITE);
    deseneaza();
}

void rotesteTaste(float x, float y, float z)
{
    bool aux=toggleRendering;
    toggleRendering=0;
    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    setcolor(DARKGRAY);
    deseneaza();

    rotatieX = x;
    rotatieY = y;
    rotatieZ = z;
    toggleRendering=aux;
    setcolor(WHITE);
    deseneaza();
}

void translateazaTaste(float x, float y, float z)
{
    bool aux=toggleRendering;
    toggleRendering=0;
    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    setcolor(DARKGRAY);
    deseneaza();

    toggleRendering=aux;
    translateaza(x,y,z);
    setcolor(WHITE);
    deseneaza();
}

void copieAnterior()
{
    for(auto pct : puncte)
        puncteAnterioare.push_back(pct);
    for(auto muchie : muchii)
        muchiiAnterioare.push_back(muchie);
    puncte.clear();
    muchii.clear();
}

void draw(string file)
{
    copieAnterior();
    ifstream fin(file);
    int x,y,z;
    fin>>n>>m;
    for(int i=1 ; i<=n ; i++)
    {
        fin>>x>>y>>z;
        puncte.push_back(punct(x, y, z));
        puncte.back().marime=1;
    }

    for(int i=1 ; i<=m ; i++)
    {
        fin>>x>>y;
        addEdge(x,y);
    }
    fin.close();
    bool aux=toggleRendering;
    toggleRendering=0;

    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    cout<<puncte.size()<<'\n';  ///important
    toggleRendering=aux;
    setcolor(WHITE);
    deseneaza();
}

void mergeObjects()
{
    int x,y,auxx;

    auxx=puncte.size();

    for(auto pct : puncteAnterioare)
        puncte.push_back(pct);

    for(auto muchie : muchiiAnterioare)
        muchii.push_back({muchie.first + auxx,muchie.second + auxx});
    puncteAnterioare.clear();
    muchiiAnterioare.clear();

    bool aux=toggleRendering;
    toggleRendering=0;

    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    setcolor(DARKGRAY);
    deseneaza();

    toggleRendering=aux;
    setcolor(WHITE);
    deseneaza();
}

void clearDrawing()
{
    puncte.clear();
    muchii.clear();
    puncteAnterioare.clear();
    muchiiAnterioare.clear();
    toggleAnimation=0;
    toggleRendering=0;
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(0,0,screenWidth,screenHeight);
}

void dimensiune(float x)
{
    bool aux=toggleRendering;
    toggleRendering=0;

    rotatieX=0;
    rotatieY=0;
    rotatieZ=0;
    setcolor(DARKGRAY);
    deseneaza();

    for(auto &pct : puncte)
        pct.marime=pct.marime*x;

    /*punct g=centruGreutate();
    translateaza(-g.x , -g.y , -g.z);
    for(auto pct : puncte)
    {
       pct.x+=x*(1-2*(pct.x>0));
       pct.y+=x*(1-2*(pct.y>0));
       pct.z+=x*(1-2*(pct.z>0));
    }
    translateaza(g.x , g.y , g.z);*/
    toggleRendering=aux;
    setcolor(WHITE);
    deseneaza();
}


void program()
{
    mainScreeen = initwindow(screenWidth + 200, screenHeight, "3D Visualizer", 200, 25);

    setcolor(DARKGRAY);
    line(screenWidth,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, DARKGRAY);
    floodfill(screenWidth-1,1,DARKGRAY);

    setfillstyle(SOLID_FILL, LIGHTGRAY);
    floodfill(screenWidth+1,1,DARKGRAY);

    button b1=button(screenWidth+9,50,screenWidth+96,100,"Draw point");
    b1.draw();

    button b2=button(screenWidth+104,50,screenWidth+191,100,"Draw edge");
    b2.draw();

    button b3=button(screenWidth+30,150,screenWidth+170,200,"Toggle animation");
    b3.draw();

    button b4=button(screenWidth+30,250,screenWidth+60,280,"C");
    b4.draw();

    button b5=button(screenWidth+85,250,screenWidth+115,280,"PP");
    b5.draw();

    button b6=button(screenWidth+140,250,screenWidth+170,280,"P");
    b6.draw();

    button b7=button(screenWidth+30,330,screenWidth+170,380,"Merge Objects");
    b7.draw();

    button b8=button(screenWidth+30,430,screenWidth+170,480,"Clear Drawing");
    b8.draw();

    button b9=button(screenWidth+30,530,screenWidth+170,580,"Toggle Rendering");
    b9.draw();

    while(true)
    {
        if(toggleAnimation)
        {
            rotatieX=0.1;
            rotatieY=0.1;
            setcolor(WHITE);
            deseneaza();
        }

        if(kbhit() && (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)))
        {
            if(toggleAnimation && !errorScreen)
                drawError("You can't rotate object","during animation!");
            else if(GetAsyncKeyState(VK_LEFT))
                rotesteTaste(0, 0.1, 0);
            else if(GetAsyncKeyState(VK_RIGHT))
                rotesteTaste(0, -0.1, 0);
            else if(GetAsyncKeyState(VK_UP))
                rotesteTaste(-0.1, 0, 0);
            else if(GetAsyncKeyState(VK_DOWN))
                rotesteTaste(0.1, 0, 0);
            else if(GetAsyncKeyState(VK_LSHIFT))
                rotesteTaste(0, 0, -0.1);
            else if(GetAsyncKeyState(VK_RSHIFT))
                rotesteTaste(0, 0, 0.1);
        }

        if(kbhit() && (GetAsyncKeyState(0x57) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x41) || GetAsyncKeyState(0x44) || GetAsyncKeyState(0x51) || GetAsyncKeyState(0x45)))
        {
            if(GetAsyncKeyState(0x57))
                translateazaTaste(0,-1,0);
            else if(GetAsyncKeyState(0x53))
                translateazaTaste(0,1,0);
            else if(GetAsyncKeyState(0x41))
                translateazaTaste(-1,0,0);
            else if(GetAsyncKeyState(0x44))
                translateazaTaste(1,0,0);
            else if(GetAsyncKeyState(0x51))
                translateazaTaste(0,0,1);
            else if(GetAsyncKeyState(0x45))
                translateazaTaste(0,0,-1);
        }

        if(kbhit() && (GetAsyncKeyState(0xA4) || GetAsyncKeyState(0xA5)) )
        {
            if(GetAsyncKeyState(0xA4))
                dimensiune(0.9);
            else dimensiune(1.1);
        }

        if(errorScreen)
            closeError();

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);

            ///vad in ce caz ma situez
            if(b1.isPressed())
                drawPoint();
            else if(b2.isPressed())
                drawEdge();
            else if(b3.isPressed())
                toggleAnimation=1-toggleAnimation;
            else if(b4.isPressed())
                draw("cube.txt");
            else if(b5.isPressed())
                draw("parallelepiped.txt");
            else if(b6.isPressed())
                draw("pyramid.txt");
            else if(b7.isPressed())
                mergeObjects();
            else if(b8.isPressed())
                clearDrawing();
            else if(b9.isPressed())
            {
                bool aux=1-toggleRendering;
                toggleRendering=0;

                rotatieX=0;
                rotatieY=0;
                rotatieZ=0;
                setcolor(DARKGRAY);
                deseneaza();
                toggleRendering=aux;
                setcolor(WHITE);
                deseneaza();
            }
        }

        this_thread::sleep_for( chrono::milliseconds( 100 / 2 ) );

        if(toggleAnimation)
        {
            rotatieX=0;
            rotatieY=0;
            rotatieZ=0;
            setcolor(DARKGRAY);
            deseneaza();
        }
    }

    getch();
    closegraph();
}

int main()
{

    program();
    return 0;
}
