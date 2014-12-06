#version 130

uniform sampler1D colormap;
uniform vec4 ds_start;
uniform vec4 ds_range;


varying vec2 pos;


//From https://thasler.com/blog/?p=93
vec2 ds_add(vec2 dsa,vec2 dsb)
{
vec2 dsc;
float t1,t2,e;
t1=dsa.x+dsb.x;
e=t1-dsa.x;
t2=((dsb.x-e)+(dsa.x-(t1-e)))+dsa.y+dsb.y;
dsc.x=t1+t2;
dsc.y=t2-(dsc.x-t1);
return dsc;
}
vec2 ds_sub(vec2 dsa,vec2 dsb)
{
vec2 dsc;
float t1,t2,e;
t1=dsa.x-dsb.x;
e=t1-dsa.x;
t2=((-dsb.x-e)+(dsa.x-(t1-e)))+dsa.y-dsb.y;
dsc.x=t1+t2;
dsc.y=t2-(dsc.x-t1);
return dsc;
}
vec2 ds_mul (vec2 dsa, vec2 dsb)
{
vec2 dsc;
float c11,c21,c2,e,t1,t2;
float a1,a2,b1,b2,cona,conb,split=8193.0;
cona=dsa.x*split;
conb=dsb.x*split;
a1=cona-(cona-dsa.x);
b1=conb-(conb-dsb.x);
a2=dsa.x-a1;
b2=dsb.x-b1;
c11=dsa.x*dsb.x;
c21=a2*b2+(a2*b1+(a1*b2+(a1*b1-c11)));
c2=dsa.x*dsb.y + dsa.y * dsb.x;
t1=c11+c2;
e=t1-c11;
t2=dsa.y*dsb.y + ((c2 - e) + (c11 - (t1 - e))) + c21;
dsc.x=t1+t2;
dsc.y=t2-(dsc.x - t1);
return dsc;
}

vec2 ds_two=vec2(2.0,0.0);
vec2 ds_four=vec2(4.0,0.0);

float mandelbrot(vec4 dsc)
{
vec4 dsz=dsc;
int i;
    for(i=0;i<256;i++)
    {
    vec2 ds_re2=ds_mul(dsz.xy,dsz.xy);
    vec2 ds_im2=ds_mul(dsz.zw,dsz.zw);
        if(ds_add(ds_re2,ds_im2).x>4.0)break;
    vec2 temp=ds_add(ds_sub(ds_re2,ds_im2),dsc.xy);
    dsz.zw=ds_mul(dsz.xy,dsz.zw);
    dsz.zw=ds_add(ds_add(dsz.zw,dsz.zw),dsc.zw);
    dsz.xy=temp;
    }
return float(i)/256.0;
}


void main()
{
vec4 dsc=vec4(ds_add(ds_start.xy,ds_mul(ds_range.xy,vec2(pos.x,0.0))),ds_add(ds_start.zw,ds_mul(ds_range.zw,vec2(pos.y,0.0))));
float result=mandelbrot(dsc);
gl_FragColor=texture1D(colormap,result);
}
