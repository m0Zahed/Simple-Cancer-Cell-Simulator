#ifndef B6065B6B_91D6_44AA_AAE8_5B8F645D1BA9
#define B6065B6B_91D6_44AA_AAE8_5B8F645D1BA9

class point{
    public: 
        float x, y;
        float r, g, b, a;
        int type; // Cancerous: 4, Non-Cancerous: 0, Medicated Cels: -60 
        int index_of_injection[2];
        point();
        point(float,float);
        void set_rgba(float,float,float,float);
};
#endif /* B6065B6B_91D6_44AA_AAE8_5B8F645D1BA9 */
