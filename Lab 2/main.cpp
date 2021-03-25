
//========== (прикладная программа) ==========
// Пополнение и использование библиотеки фигур
//#include "pch.h"	//связь с ОС (пример для Visual C++2017)
#include "screen.h"
#include "shape.h"
// ПРИМЕР ДОБАВКИ: дополнительный фрагмент - полуокружность
class h_circle: public rectangle, public reflectable {
   // bool reflected;
public:
    h_circle(point a, point b) : rectangle(a, b)  {Set_State_R(); }//bool r=true; reflected(r)
    void draw();
    void flip_horisontally( ) { }; // Отразить горизонтально (пустая функция)
    void flip_vertically( ) { this->Set_State_R(); };	// Отразить вертикально
};
void h_circle :: draw() //Алгоритм Брезенхэма для окружностей
{   //(выдаются два сектора, указываемые значением reflected)
    int x0 = (sw.x + ne.x)/2, y0 = this->Get_State_R() ? sw.y : ne.y;//reflected
    int radius = (ne.x - sw.x)/2;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while(y >= 0) { // Цикл рисования
        if(this->Get_State_R()) { put_point(x0 + x, y0 + y*0.7); put_point(x0 - x, y0 + y*0.7); }
        else { put_point(x0 + x, y0 - y*0.7); put_point(x0 - x, y0 - y*0.7); }
        error = 2 * (delta + y) - 1;
        if(delta < 0 && error <= 0) { ++x; delta += 2 * x + 1; continue; }
        error = 2 * (delta - x) - 1;
        if(delta > 0 && error > 0) { --y; delta += 1 - 2 * y; continue; }
        ++x; delta += 2 * (x - y);  --y;
    }
}

//квадрат
class square: public shape {
/* nw--- n ---ne
   |          |
   w     c    e
   |  	      |
   sw--- s ---se */
protected:
    point sw, ne;
public:
    square(point a,int sz) :  sw(a) {ne.x = sw.x + sz; ne.y = sw.y + sz;}
    point north( ) const { return point((sw.x + ne.x) / 2, ne.y); }
    point south( ) const { return point((sw.x + ne.x) / 2, sw.y); }
    point east( ) const { return point(ne.x, (sw.y + ne.y) / 2); }
    point west( ) const { return point(sw.x, (sw.y + ne.y) / 2); }
    point neast( ) const { return ne; }
    point seast( ) const { return point(ne.x, sw.y); }
    point nwest( ) const { return point(sw.x, ne.y); }
    point swest( ) const { return sw; }
    void move(int a, int b)
    { sw.x += a; sw.y += b; ne.x += a; ne.y += b; }
    void resize(int d)
    { ne.x += (ne.x - sw.x) * (d - 1); ne.y += (ne.y - sw.y) * (d - 1); }
    void draw( )
    {
        put_line(nwest( ), ne);   put_line(ne, seast( ));
        put_line(seast( ), sw);   put_line(sw, nwest( ));
    }
};

// ПРИМЕР ДОБАВКИ: дополнительная функция присоединения…
void down(shape &p,  const shape &q)
{    point n = q.south( );
    point s = p.north( );
    p.move(n.x - s.x, n.y - s.y - 1); }
void  left(shape &p,  const shape &q)
{
    point w = q.west();
    point e = p.east();
    p.move(w.x - e.x, w.y - e.y);
}
void right(shape &p,  const shape &q)
{
    point e = q.east();
    point w = p.west();
    p.move(e.x - w.x , e.y - w.y);
}
void rghtcorn(shape &p,  const shape &q)
{
    point ne = q.neast();
    point sw = p.swest();
    p.move(ne.x - sw.x, ne.y - sw.y - 1);
}
void lftcorn(shape &p,  const shape &q)
{
    point nw = q.nwest();
    point se = p.seast();
    p.move(nw.x - se.x, nw.y - se.y - 1  );
}
void  CloneNCon (shape &p1,shape &p2, const shape &q, int num)
{
   if(num){left(p1,q); right(p2,q);}
   else{lftcorn(p1,q);rghtcorn(p2,q);}
}
// Cборная пользовательская фигура - физиономия
class myshape : public rectangle { // Моя фигура ЯВЛЯЕТСЯ
    int w, h;			             //        прямоугольником
    line l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line r_eye; // правый глаз
    line mouth; // рот
    square nose; // НОС !!!
public:
    myshape(point, point);
    void draw( );
    void move(int, int);
    void resize(int) { }
};
myshape :: myshape(point a, point b)
        : rectangle(a, b),	//Инициализация базового класса
          w(neast( ).x - swest( ).x + 1), // Инициализация данных
          h(neast( ).y - swest( ).y + 1), // - строго в порядке объявления!
          l_eye(point(swest( ).x + 2, swest( ).y + h * 3 / 4), 2),
          r_eye(point(swest( ).x + w - 4, swest( ).y + h * 3 / 4), 2),
          mouth(point(swest( ).x + 2, swest( ).y + h / 4), w - 4),
          nose(point(b.x-a.x/2+1,b.y-a.y/2),2)
{ }
void myshape :: draw( )
{
    rectangle :: draw( ); //Контур лица (глаза и нос рисуются сами!)
  //  int a = (swest( ).x + neast( ).x) / 2;
   // int b = (swest( ).y + neast( ).y) / 2;
    //put_point(point(a, b)); // Нос – существует только на рисунке!
}
void myshape :: move(int a, int b)
{
    rectangle :: move(a, b);
    l_eye.move(a, b);  r_eye.move(a, b);
            nose.move(a,b);
    mouth.move(a, b);
}
int main( )
{   setlocale(LC_ALL, "Rus");
    screen_init( );
    Message M;
//== 1.Объявление набора фигур ==
    rectangle hat(point(8, 8), point(14,14 ));//st 0 0 14 5
    line brim(point(5,0),17);//st .. 15 17
    myshape face(point(25,10), point(45,18));//st .. 27 18
    h_circle beard(point(55,10), point(70,20));//st 40 10  50 20
    square ear_1(point(5,10),2);
    square ear_2(point(8,10),2);
    square feather_1(point(50,5),3);
    square feather_2(point(55,5),3);
    shape_refresh(&M);
    std::cout << "=== Generated... ===\n";
    M.Give_Message();
    std::cin.get(); //Смотреть исходный набор
//== 2.Подготовка к сборке ==
    Rotate_Check_Error(&hat,'r',&M);
  //  hat.rotate_right( ); //поворот шляпы
    brim.resize(2);
    face.resize(2);
    Reflect_Check_Error(&beard,'v',&M);
   // beard.flip_vertically();
    shape_refresh(&M);
    std::cout << "=== Prepared... ===\n";
    M.Give_Message();
    std::cin.get(); //Смотреть результат поворотов/отражений
//== 3.Сборка изображения ==
//	face.move(0, -10); // Лицо - в исходное положение
    up(brim, face);
    up(hat, brim);
    down(beard, face);
    CloneNCon(ear_1,ear_2,face,1);
    CloneNCon(feather_1,feather_2,hat,0);
    shape_refresh(&M);
    std::cout << "=== Ready! ===\n";
   // M.Give_Message();
    std::cin.get(); //Смотреть результат
    screen_destroy( );
    return 0;
}
