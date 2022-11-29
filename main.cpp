#include <bits/stdc++.h>

using namespace std;

struct punct
{
    float x, y, z;

    punct(){}

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

void citire(string &address)
{
    FILE *input = fopen(address.c_str(), "rb");

    fread(&n, 4, 1, input);
    fread(&m, 4, 1, input);

    for(int i=0;i<n;i++)
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

    for(int i=0;i<n;i++)
    {
        fwrite(&puncte[i].x, 4, 1, output);
        fwrite(&puncte[i].y, 4, 1, output);
        fwrite(&puncte[i].z, 4, 1, output);
    }

    for(int i=0;i<m;i++)
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

    for(int i=0;i<n;i++)
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
    for(int i=0;i<n;i++)
    {
        puncte[i].x += dx;
        puncte[i].y += dy;
        puncte[i].z += dz;
    }
}



int main()
{

    return 0;
}
