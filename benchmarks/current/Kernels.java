import java.util.*;
import java.nio.charset.StandardCharsets;

/** 相同输入和 UTF-8 字节任务的 Java 对照，预热与正式样本分别记录。 */
public final class Kernels {
    static final long MASK=2147483647L;
    static volatile long blackhole;
    record Vec(long x,long y,long z) {}
    record Transform(Vec position,Vec velocity,long[] weights) {}
    record Row(Transform transform,long[] payload,long flags) {}
    record Event(int tag,long x,long y) {}
    interface Operation { long apply(long v); }
    static final class Multiply implements Operation { public long apply(long v){return(v*3+7)&MASK;} }
    static final class Subtract implements Operation { public long apply(long v){return(v*5-11)&MASK;} }
    static final class ExclusiveOr implements Operation { public long apply(long v){return((v*7)^13)&MASK;} }
    static final Operation[] ops={new Multiply(),new Subtract(),new ExclusiveOr(),new Multiply()};
    static final Row[] rows=new Row[256];
    static final Event[] events=new Event[256];
    static final long[][] matrix=new long[64][64];
    static final long[][][] cube=new long[16][16][16];
    static final byte[] unit="ChaLi-字符串-🙂-0123456789|".getBytes(StandardCharsets.UTF_8);
    static final byte[] first=Arrays.copyOf(unit,unit.length+1),second=Arrays.copyOf(unit,unit.length+1);
    static final byte[] text=new byte[256*(unit.length+1)];
    static final byte[][] needles={"字符串".getBytes(StandardCharsets.UTF_8),"🙂".getBytes(StandardCharsets.UTF_8),"不存在!".getBytes(StandardCharsets.UTF_8),"012345".getBytes(StandardCharsets.UTF_8)};
    static {
        first[unit.length]='a';second[unit.length]='B';
        for(int i=0;i<256;i++){
            System.arraycopy(unit,0,text,i*first.length,unit.length);text[i*first.length+unit.length]=(byte)(65+i%26);
            events[i]=new Event(i%4,i%4==1?i*3+7:i%4==2?i*5:i%4==3?i*7:0,i%4==2?i+11:i%4==3?i+13:0);
            long[] w=new long[4],p=new long[8];for(int k=0;k<4;k++)w[k]=i*13+k;for(int k=0;k<8;k++)p[k]=i*17+k*3;
            rows[i]=new Row(new Transform(new Vec(i*3+1,i*5+2,i*7+3),new Vec(i+4,i+5,i+6),w),p,i*11+9);
        }
        for(int r=0;r<64;r++)for(int c=0;c<64;c++)matrix[r][c]=r*131+c*17+3;
        for(int x=0;x<16;x++)for(int y=0;y<16;y++)for(int z=0;z<16;z++)cube[x][y][z]=x*257+y*31+z*7+3;
    }
    static long key(long i,long s){return(i*65537+s*17)%1000000007+1000000007;}
    static long mix(long v){return v*17+3;}
    static int find(byte[] needle,int start){outer:for(int i=start;i<=text.length-needle.length;i++){for(int j=0;j<needle.length;j++)if(text[i+j]!=needle[j])continue outer;return i;}return -1;}
    static long work(String name,long s){
        long a=s;
        switch(name){
        case "int_chain":for(long i=0;i<4000000;i++)a=(a*48271+i)&MASK;break;
        case "call":for(long i=0;i<1000000;i++)a=(mix(a)+i)&MASK;break;
        case "array":case "array_rw":{
            long[] v=new long[16];for(int k=0;k<16;k++)v[k]=k*3;
            if(name.equals("array")){for(int i=0;i<4000000;i++)a=(a+v[(int)(a&15)])&MASK;}
            else{for(long i=0;i<4000000;i++){int k=(int)(a&15);v[k]=(v[k]+i)&MASK;a=(a+v[k])&MASK;}}break;}
        case "list":case "list_growth":{
            ArrayList<Long> v=new ArrayList<>();for(long i=0;i<400000;i++)v.add(i*3+s);for(int i=0;i<v.size();i++)a=(a*31+v.get(i))&MASK;break;}
        case "map_growth":{
            HashMap<Long,Long> v=new HashMap<>();for(long i=0;i<20000;i++)v.put(key(i,s),i*3+s);
            for(long i=0;i<20000;i++){long k=key(i,s);a=(a*31+v.get(k))&MASK;v.put(k,i*5+s);}
            for(long i=0;i<20000;i++){long k=key(i,s);a=(a*31+v.get(k)+(v.containsKey(-k)?1:0))&MASK;}
            for(long i=0;i<20000;i+=3){v.remove(key(i,s));a++;}for(long i=0;i<20000;i+=3)v.put(key(i,s),i*7+s);for(long i=0;i<20000;i++)a=(a*31+v.get(key(i,s)))&MASK;break;}
        case "set_growth":{
            HashSet<Long> v=new HashSet<>();for(int j=0;j<2;j++)for(long i=0;i<20000;i++)v.add(key(i,s));
            for(long i=0;i<20000;i++){long k=key(i,s);a=(a*31+(v.contains(k)?1:0)+(v.contains(-k)?2:0))&MASK;}
            for(long i=0;i<20000;i+=3){v.remove(key(i,s));a++;}for(long i=0;i<20000;i+=3)v.add(key(i,s));for(long i=0;i<20000;i++)a=(a*31+(v.contains(key(i,s))?1:0))&MASK;break;}
        case "enum_payload":for(long i=0;i<262144;i++){Event e=events[(int)((i*13+((a>>7)&255))&255)];a=(a*33+(e.tag==0?17:e.x+e.y))&MASK;}break;
        case "interface_mono":for(int i=0;i<262144;i++)a=ops[0].apply(a);break;
        case "interface_poly":for(long i=0;i<262144;i++)a=ops[(int)((i+s)&3)].apply(a);break;
        case "struct_nested":for(long i=0;i<262144;i++){Row r=rows[(int)((i*13+(a&255))&255)];Transform t=r.transform;long v=t.position.x+t.position.z+t.velocity.y+t.weights[(int)(i&3)]+r.payload[(int)((i+s)&7)]+r.flags;a=(a*33+v)&MASK;}break;
        case "array_2d":for(long i=0;i<262144;i++)a=(a*33+matrix[(int)((a>>6)&63)][(int)((i+a)&63)])&MASK;break;
        case "array_3d":for(long i=0;i<262144;i++)a=(a*33+cube[(int)((a>>4)&15)][(int)((a>>8)&15)][(int)((i+s)&15)])&MASK;break;
        case "string_read":for(long i=0;i<131072;i++)a=(a*33+Byte.toUnsignedInt(text[(int)((i*13+((a>>7)&65535))%text.length)]))&MASK;break;
        case "string_concat":{
            byte[] v=new byte[0];for(int i=0;i<2048;i++){byte[] chunk=((i+s)&1)==0?first:second;byte[] next=Arrays.copyOf(v,v.length+chunk.length);System.arraycopy(chunk,0,next,v.length,chunk.length);v=next;}for(byte b:v)a=(a*33+Byte.toUnsignedInt(b))&MASK;break;}
        case "string_slice":for(int i=0;i<1024;i++){int block=(int)((i*13+(a&127))&127),start=block*first.length,width=(int)((((i+s)&3)+1)*first.length),other=(i&1)==0?start:((block+13)&127)*first.length;byte[] p=Arrays.copyOfRange(text,start,start+width),q=Arrays.copyOfRange(text,other,other+width);a=(a*33+Byte.toUnsignedInt(p[0])+Byte.toUnsignedInt(p[p.length-1])+(Arrays.equals(p,q)?1:0))&MASK;}break;
        case "string_find":for(int i=0;i<256;i++){int start=(int)((((a>>7)+i)&255)*first.length);a=(a*33+find(needles[(int)((i+s)&3)],start)+1)&MASK;}break;
        default:throw new IllegalArgumentException(name);
        }return a;
    }
    static double floating(long s){double x=s;for(int i=0;i<1000000;i++)x=x*1.0000001+0.0000001;return x;}
    public static void main(String[] args){
        String name=args[0];boolean fp=name.equals("float_chain");
        if(args.length>1&&args[1].equals("--verify")){for(int s=0;s<5;s++)System.out.println("value "+s+" "+(fp?Double.toString(floating(s)):Long.toString(work(name,s))));return;}
        long before=System.nanoTime();int calls=0;
        do{blackhole=fp?Double.doubleToLongBits(floating(calls%5)):work(name,calls%5);calls++;}while(calls<32||System.nanoTime()-before<1000000000L);
        System.out.println("warmup_calls "+calls);System.out.println("warmup_ns "+(System.nanoTime()-before));
        if(fp){double sink=floating(0);for(int s=0;s<5;s++){long start=System.nanoTime();double v=floating(s);long elapsed=System.nanoTime()-start;sink=sink*0.5+v;System.out.println("rep "+elapsed);}System.out.println("sink "+sink);}
        else{long sink=work(name,0);for(int s=0;s<5;s++){long start=System.nanoTime();long v=work(name,s);long elapsed=System.nanoTime()-start;sink+=v;System.out.println("rep "+elapsed);}blackhole=sink;System.out.println("sink "+sink);}
    }
}
