
//========== (прикладная программа) ==========
// Пополнение и использование библиотеки фигур
//#include "pch.h"	//связь с ОС (пример для Visual C++2017)
#include "screen.h"
#include "shape.h"
// ПРИМЕР ДОБАВКИ: дополнительный фрагмент - полуокружность
class h_circle: public rectangle, public reflectable {
    bool reflected;
public:
    h_circle(point a, point b, bool r=true) : rectangle(a, b), reflected(r) { }
    void draw();
    void flip_horisontally( ) { }; // Отразить горизонтально (пустая функция)
    void flip_vertically( ) { reflected = !reflected; };	// Отразить вертикально
};
void h_circle :: draw() //Алгоритм Брезенхэма для окружностей
{   //(выдаются два сектора, указываемые значением reflected)
    int x0 = (sw.x + ne.x)/2, y0 = reflected ? sw.y : ne.y;
    int radius = (ne.x - sw.x)/2;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while(y >= 0) { // Цикл рисования
        if(reflected) { put_point(x0 + x, y0 + y*0.7); put_point(x0 - x, y0 + y*0.7); }
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
    point n = q.south( );
    point s = p.north( );
    point w = q.east( );
    point e = p.west( );
    p.move(w.x - e.x, n.y - s.y/2);
}

void right(shape &p,  const shape &q)
{
    point n = q.south( );
    point s = p.north( );
    point e = q.west( );
    point w = p.east( );
    p.move(e.x - w.x, n.y - s.y/2);
}
// Cборная пользовательская фигура - физиономия
class myshape : public rectangle { // Моя фигура ЯВЛЯЕТСЯ
    int w, h;			             //        прямоугольником
    line l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line r_eye; // правый глаз
    line mouth; // рот
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
          mouth(point(swest( ).x + 2, swest( ).y + h / 4), w - 4)
{ }
void myshape :: draw( )
{
    rectangle :: draw( ); //Контур лица (глаза и нос рисуются сами!)
    int a = (swest( ).x + neast( ).x) / 2;
    int b = (swest( ).y + neast( ).y) / 2;
    put_point(point(a, b)); // Нос – существует только на рисунке!
}
void myshape :: move(int a, int b)
{
    rectangle :: move(a, b);
    l_eye.move(a, b);  r_eye.move(a, b);
    mouth.move(a, b);
}
int main( )
{   setlocale(LC_ALL, "Rus");
    screen_init( );
//== 1.Объявление набора фигур ==
    rectangle hat(point(0, 0), point(10, 8));//st 0 0 14 5
    line brim(point(0,15),17);//st .. 15 17
    myshape face(point(15,10), point(35,18));//st .. 27 18
    h_circle beard(point(35,10), point(50,20));//st 40 10  50 20
    shape_refresh( );
    std::cout << "=== Generated... ===\n";
    std::cin.get(); //Смотреть исходный набор
//== 2.Подготовка к сборке ==
    hat.rotate_right( );
    brim.resize(2);
    face.resize(2);
    beard.flip_vertically();
    shape_refresh( );
    std::cout << "=== Prepared... ===\n";
    std::cin.get(); //Смотреть результат поворотов/отражений
//== 3.Сборка изображения ==
//	face.move(0, -10); // Лицо - в исходное положение
    up(brim, face);
    up(hat, brim);
    down(beard, face);
    shape_refresh( );
    std::cout << "=== Ready! ===\n";
    std::cin.get(); //Смотреть результат
    screen_destroy( );
    return 0;
}
