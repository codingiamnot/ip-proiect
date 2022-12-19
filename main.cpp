#include <bits/stdc++.h>
#include <graphics.h>
#include <winbgim.h>
#include <ctime>

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

vector<bool> viz;

float procentDeOcupareEcran = 0.75;

float rotatieX = 0, rotatieY =0;

bool animOn = true, toggleAnimation = true;

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

bool compZ(pair<punct, int> a, pair<punct, int> b)
{
    return a.first.z < b.first.z;
}

bool isInside(punct x, vector<punct> &v)
{
    return false;
}

void reorder(vector<punct> &v)
{
    vector<punct> ans;


}

void calcViz()
{
    viz.resize((int)puncte.size());

    vector< pair<punct, int> > v;

    vector< punct > poligon;

    for(int i=0; i<(int)puncte.size(); i++)
        v.push_back( {puncte[i], i} );

    sort(v.begin(), v.end(), compZ);

    for(auto pct : v)
    {
        if(isInside(pct.first, poligon))
            viz[ pct.second ] = false;

        else
        {
            viz[ pct.second ] = true;
            poligon.push_back( pct.first );

            reorder(poligon);
        }
    }

    /*
    for(int i=0; i<(int)v.size(); i++)
        cout<<viz[i];
    cout<<endl;
    */
}


void deseneaza(float d = 10)
{
    ///calc puncte vizibile
    calcViz();

    ///calc rotatie

    vector<punct> puncteRotite;
    punct g = centruGreutate();


    for(auto pct : puncte)
    {
        float x = pct.x , y = pct.y, z = pct.z;

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


        ///translatam
        x += g.x;
        y += g.y;
        z += g.z;

        puncteRotite.push_back(punct(x, y, z));
    }


    ///calc plane geometric values
    vector<punct> punctePlan;

    for(auto pct : puncteRotite)
    {
        float x = (pct.x / pct.z) * d;
        float y = (pct.y / pct.z) * d;

        punctePlan.push_back( punct(x, y, 0) );
    }

    ///calc screen values
    vector< pair<int, int> > puncteEcran;

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
        int x = ( ((pct.x - minX) / (maxX - minX)) - 0.5 ) * (float)screenHeight * procentDeOcupareEcran + screenHeight/2;
        int y = ( ((pct.y - minY) / (maxY - minY)) - 0.5 ) * (float)screenWidth * procentDeOcupareEcran + screenWidth / 2;

        //cout<<pct.x<<' '<<pct.y<<" becomes "<<x<<' '<<y<<endl;

        puncteEcran.push_back( {x, y} );
    }

    //initwindow(screenWidth, screenHeight);

    for(int i=0;i<(int)muchii.size();i++)
    {
        int id1 = muchii[i].first;
        int id2 = muchii[i].second;

        if(!viz[id1] || !viz[id2])
            continue;

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

    roteste(0.1, 0.1);

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

    ///initwindow(screenWidth , screenHeight);


    while(true)
    {
        if(animOn)
            roteste(0.1, 0.1);

        ///cleardevice();


        setcolor(WHITE);
        deseneaza();

        this_thread::sleep_for( chrono::milliseconds( 100 / 3 ) );

        setcolor(DARKGRAY);
        deseneaza();
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
            //setcolor(BLACK);
            //setbkcolor(BLACK);
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

void meniu()  ///nu e util
{
    initwindow(screenWidth + 200, screenHeight);

    setcolor(DARKGRAY);
    line(screenWidth,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, DARKGRAY);
    floodfill(screenWidth-1,1,DARKGRAY);

    setfillstyle(SOLID_FILL, LIGHTGRAY);
    floodfill(screenWidth+1,1,DARKGRAY);

    button b1=button(screenWidth+50,50,screenWidth+150,100,"Draw point");
    b1.draw();

    button b2=button(screenWidth+50,150,screenWidth+150,200,"Draw edge");
    b2.draw();

    button b3=button(screenWidth+30,screenHeight-100,screenWidth+170,screenHeight-50,"Toggle animation");
    b3.draw();

    setcolor(LIGHTGRAY);
    mainLoop();

    getch();
    closegraph();
}

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
    //getch();
    ///closegraph(errorScreen);
    //outtextxy(screenWidth + 100, screenHeight - 90 + 5, (char *)text1.c_str());
    //outtextxy(screenWidth + 100, screenHeight - 70 + 5, (char *)text2.c_str());

}

void closeError()
{
    setcurrentwindow(errorScreen);
    if(ismouseclick(WM_LBUTTONDOWN))
        clearmouseclick(WM_LBUTTONDOWN) , closegraph(errorScreen) , errorScreen=0;
    setcurrentwindow(mainScreeen);
}

void coverError() //nu ma e bun
{
    setfillstyle(SOLID_FILL,LIGHTGRAY);
    bar(screenWidth , screenHeight - 100 , screenWidth + 200 , screenHeight);
}

void drawPoint2() ///gresit
{
    if(toggleAnimation)
    {
        drawError("You can't draw ","during animation!\n");
        return;
    }

    bool drawn=0;
    clock_t currTime=0;

    while(!drawn)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            int x=mousex(), y=mousey();
            if(x>=screenWidth && x<=screenWidth+200 && y>=0 && y<=screenHeight)
                drawError("You can't draw ","a point there!\n") , currTime=clock();
            else
            {
                drawn = 1;
                coverError();
                ///functie de desenare
            }
        }
        if((float)clock()/CLOCKS_PER_SEC - (float)currTime/CLOCKS_PER_SEC >= 1.5)
                coverError();
    }
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
            int x=mousex(), y=mousey();

            if(x>=screenWidth && x<=screenWidth+200 && y>=0 && y<=screenHeight)
                drawError("You can't draw ","a point there!\n");
            else
            {
                drawn = 1;
                ///functie de desenare
            }
        }

    }
}

void drawEdge()
{
     if(toggleAnimation)
    {
        drawError("You can't draw ","during animation!\n");
        return;
    }

    ///
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

    button b1=button(screenWidth+50,50,screenWidth+150,100,"Draw point");
    b1.draw();

    button b2=button(screenWidth+50,150,screenWidth+150,200,"Draw edge");
    b2.draw();

    button b3=button(screenWidth+30,250,screenWidth+170,300,"Toggle animation");
    b3.draw();

    while(true)
    {
        if(toggleAnimation)
        {
            if(animOn)
                roteste(0.1, 0.1);
            setcolor(WHITE);
            deseneaza();
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
        }

        this_thread::sleep_for( chrono::milliseconds( 100 / 2 ) );

        if(toggleAnimation)
        {
            setcolor(DARKGRAY);
            deseneaza();
        }
    }

    getch();
    closegraph();
}


int main()
{
    //test();
    //mainLoop();
    //meniu();
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

    program();
    return 0;
}
