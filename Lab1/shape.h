//
// Created by Vik_S on 26.02.2021.
//

#ifndef REALTASKOFAPPOINTMENTS_SHAPE_H
#define REALTASKOFAPPOINTMENTS_SHAPE_H
#include "iostream"
#include <list>
#include "screen.h"

using namespace std;
//==1. Поддержка экрана в форме матрицы символов ==
    char screen[YMAX] [XMAX];
    enum color { black = '*', white = '.' };
    void screen_init( )
    {
        for(auto y = 0; y < YMAX; ++y)
            for (auto &x : screen[y])  x = white;
    }
    void screen_destroy( )
    {
        for(auto y = 0; y < YMAX; ++y)
            for (auto &x : screen[y])  x = black;
    }
    bool on_screen(int a, int b) // проверка попадания точки на экран
    { return 0 <= a && a < XMAX && 0 <= b && b < YMAX; }
    void put_point(int a, int b)
    { if (on_screen(a,b)) screen[b] [a] = black; }
    void put_line(int x0, int y0, int x1, int y1)
/* Алгоритм Брезенхэма для прямой:
рисование отрезка прямой от (x0,y0) до (x1,y1).
Уравнение прямой: b(x-x0) + a(y-y0) = 0.
Минимизируется величина abs(eps), где eps = 2*(b(x-x0)) + a(y-y0).  */
    {
        int dx = 1;
        int a = x1 - x0;   if (a < 0) dx = -1, a = -a;
        int dy = 1;
        int b = y1 - y0;   if (b < 0) dy = -1, b = -b;
        int two_a = 2*a;
        int two_b = 2*b;
        int xcrit = -b + two_a;
        int eps = 0;
        for (;;) { //Формирование прямой линии по точкам
            put_point(x0, y0);
            if (x0 == x1 && y0 == y1) break;
            if (eps <= xcrit) x0 += dx, eps += two_b;
            if (eps >= a || a < b) y0 += dy, eps -= two_a;
        }
    }
    void screen_clear( ) { screen_init( ); } //Очистка экрана
    void screen_refresh( ) // Обновление экрана
    {
        for (int y = YMAX-1; 0 <= y; --y) { // с верхней строки до нижней
            for (auto x : screen[y])    // от левого столбца до правого
                std::cout << x;
            std::cout << '\n';
        }
    }
//== 2. Библиотека фигур ==
    struct shape { // Виртуальный базовый класс "фигура"
        static list<shape*> shapes;// Список фигур (один на все фигуры!)
        shape( ) { shapes.push_back(this); } //Фигура присоединяется к списку
        virtual point north( ) const = 0;	//Точки для привязки
        virtual point south( ) const = 0;
        virtual point east( ) const = 0;
        virtual point west( ) const = 0;
        virtual point neast( ) const = 0;
        virtual point seast( ) const = 0;
        virtual point nwest( ) const = 0;
        virtual point swest( ) const = 0;
        virtual void draw( ) = 0;		//Рисование
        virtual void move(int, int) = 0;	//Перемещение
        virtual void resize(int) = 0;    	//Изменение размера
    };
    list<shape*> shape::shapes;   // Размещение списка фигур
    void shape_refresh( ) // Перерисовка всех фигур на экране
    {
        screen_clear( );
        for (auto p : shape :: shapes) p->draw( ); //Динамическое связывание!!!
        screen_refresh( );
    }
    class rotatable : virtual public shape { //Фигуры, пригодные к повороту
    public:
        virtual void rotate_left( ) = 0;	//Повернуть влево
        virtual void rotate_right( ) = 0;	//Повернуть вправо
    };
    class reflectable : virtual public shape { // Фигуры, пригодные
    public:					     // к зеркальному отражению
        virtual void flip_horisontally( ) = 0;	// Отразить горизонтально
        virtual void flip_vertically( ) = 0;	          // Отразить вертикально
    };

//прямая линия (отрезок)   -----------
    class line : public shape {
/* отрезок прямой ["w", "e"].
   north( ) определяет точку "выше центра отрезка и так далеко
   на север, как самая его северная точка", и т. п. */
    protected:
        point w, e;
    public:
        line(point a, point b) : w(a), e(b) { };
        line(point a, int L) : w(point(a.x + L - 1, a.y)), e(a) {  };
        point north( ) const { return point((w.x+e.x)/2, e.y<w.y? w.y : e.y); }
        point south( ) const { return point((w.x+e.x)/2, e.y<w.y? e.y : w.y); }
        point east( ) const { return point(e.x<w.x? w.x : e.x, (w.y+e.y)/2); }
        point west( ) const { return point(e.x<w.x? e.x : w.x, (w.y+e.y)/2); }
        point neast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? w.y : e.y); }
        point seast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? e.y : w.y); }
        point nwest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? w.y : e.y); }
        point swest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? e.y : w.y); }
        void move(int a, int b) 	{ w.x += a; w.y += b; e.x += a; e.y += b; }
        void draw( ) { put_line(w, e); }
        void resize(int d) // Увеличение длины линии в (d) раз
        { e.x += (e.x - w.x) * (d - 1); e.y += (e.y - w.y) * (d - 1); }
    };
// Прямоугольник
    class rectangle : public rotatable {
/* nw----- n -----ne
   |              |
   w       c      e
   |  	          |
   sw----- s -----se */
    protected:
        point sw, ne;
    public:
        rectangle(point a, point b) :  sw(a), ne(b) { }
        point north( ) const { return point((sw.x + ne.x) / 2, ne.y); }
        point south( ) const { return point((sw.x + ne.x) / 2, sw.y); }
        point east( ) const { return point(ne.x, (sw.y + ne.y) / 2); }
        point west( ) const { return point(sw.x, (sw.y + ne.y) / 2); }
        point neast( ) const { return ne; }
        point seast( ) const { return point(ne.x, sw.y); }
        point nwest( ) const { return point(sw.x, ne.y); }
        point swest( ) const { return sw; }
        void rotate_right() // Поворот вправо относительно se
        { int w = ne.x - sw.x, h = ne.y - sw.y; //(учитывается масштаб по осям)
            sw.x = ne.x - h * 2; ne.y = sw.y + w / 2;	}
        void rotate_left() // Поворот влево относительно sw
        { int w = ne.x - sw.x, h = ne.y - sw.y;
            ne.x = sw.x + h * 2; ne.y = sw.y + w / 2; }
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
    void up(shape& p, const shape& q) // поместить p над q
    {	//Это ОБЫЧНАЯ функция, не член класса! Динамическое связывание!!
        point n = q.north( );
        point s = p.south( );
        p.move(n.x - s.x, n.y - s.y + 1);
    }



#endif //REALTASKOFAPPOINTMENTS_SHAPE_H