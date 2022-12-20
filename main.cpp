#include <bits/stdc++.h>
#include <graphics.h>
#include <winbgim.h>
#include <ctime>

using namespace std;

vector<long double> dy;



const int screenWidth = 600, screenHeight = 600;
long double er = 1e-7;

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

float rotatieX = 0, rotatieY =0 , rotatieZ = 0;

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
    for(int i=0; i<puncte.size(); i++)
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

void roteste(float alfa, float beta, float omega)
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
    rotatieZ += omega;
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

bool isInside(punct a, vector<punct> &v)
{
    int ans = 0;

    for(auto pct : v)
        if(pct.x > a.x && abs(pct.y - a.y) <= er)
            ans ++;


    for(int i=0; i<(int)v.size()-1; i++)
    {
        if( abs(v[i].x - v[i+1].x) <= er )
        {
            if( min(v[i].y, v[i+1].y) + er <= a.y && a.y + er <= max(v[i].y, v[i].y) )
                ans++;
        }

        else
        {
            long double dy = (v[i].y - v[i+1].y) / (v[i].x - v[i+1].x);
            long double x = (a.y - v[i].y) / dy + v[i].x;

            if( min(v[i].x, v[i+1].x) + er <= x && x + er <= min(v[i].x, v[i+1].x) )
                ans++;
        }

    }

    return (ans % 2);
}

bool compPanta(int i, int j)
{
    return dy[i] < dy[j];
}

void reorder(vector<punct> &v)
{
    int minP = 0;
    for(int i=1; i<(int)v.size(); i++)
    {
        if(v[i].x < v[minP].x || (abs(v[i].x - v[minP].x) <= er && v[i].y < v[minP].y) )
            minP = i;
    }

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

    vector<int> perm;
    for(int i=0; i<(int)v.size(); i++)
        perm.push_back(i);
    sort(perm.begin(), perm.end(), compPanta);

    vector<punct> ans;

    for(int i=0; i<(int)v.size(); i++)
        ans.push_back(v[i]);

    v = ans;
}


void calcViz(vector<punct> &puncte)
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

        //if(!viz[id1] || !viz[id2])
          //  continue;

        line( puncteEcran[ id1 ].first, puncteEcran[ id1 ].second, puncteEcran[ id2 ].first, puncteEcran[ id2 ].second );
    }

    for(auto pct : puncteEcran)
    {
        putpixel((int)pct.first, (int)pct.second, getcolor());
    }

    //getch();
    //closegraph();
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

                puncte.push_back(punct(x,y,G.z));

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

void rotesteTaste(float x, float y, float z)
{
    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    setcolor(DARKGRAY);
    deseneaza();

    rotatieX = x;
    rotatieY = y;
    rotatieZ = z;
    setcolor(WHITE);
    deseneaza();
}

void translateazaTaste(float x, float y, float z)
{
    rotatieX = 0;
    rotatieY = 0;
    rotatieZ = 0;
    setcolor(DARKGRAY);
    deseneaza();

    translateaza(x,y,z);
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
            //roteste(0.1, 0.1);
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
                rotesteTaste(0 , 0.1 , 0);
            else if(GetAsyncKeyState(VK_RIGHT))
                rotesteTaste(0 , -0.1 , 0);
            else if(GetAsyncKeyState(VK_UP))
                rotesteTaste(-0.1 , 0 , 0);
            else if(GetAsyncKeyState(VK_DOWN))
                rotesteTaste(0.1 , 0 , 0);
            else if(GetAsyncKeyState(VK_LSHIFT))
                rotesteTaste(0 , 0 , -0.1);
            else if(GetAsyncKeyState(VK_RSHIFT))
                rotesteTaste(0 , 0 , 0.1);
        }

        if(kbhit() && (GetAsyncKeyState(0x57) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x41) || GetAsyncKeyState(0x44) || GetAsyncKeyState(0x51) || GetAsyncKeyState(0x45)))
        {
             if(toggleAnimation && !errorScreen)
                drawError("You can't move object","during animation!");
            else if(GetAsyncKeyState(0x57))
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
