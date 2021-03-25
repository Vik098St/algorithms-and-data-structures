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
        //~shape(){while(!shapes.empty())shapes.pop_front();}
    };

//== 2.1  Класс исключительных ситуаций
class CrossError: public shape, exception
{
protected:
    point sw = point(0,YMAX-8);
    point ne = point(sw.x+9,YMAX-1);
public:
    CrossError()=default;
    point  north( ) const final { return point((sw.x + ne.x) / 2, ne.y); }
    point south( ) const final { return point((sw.x + ne.x) / 2, sw.y); }
    point east( ) const final{ return point(ne.x, (sw.y + ne.y) / 2); }
    point west( ) const final { return point(sw.x, (sw.y + ne.y) / 2); }
    point neast( ) const final { return ne; }
    point seast( ) const final { return point(ne.x, sw.y); }
    point nwest( ) const final { return point(sw.x, ne.y); }
    point swest( ) const final { return sw; }
    void move(int a, int b) override
    { sw.x += a; sw.y += b; ne.x += a; ne.y += b; }
    void resize(int d)
    { ne.x += (ne.x - sw.x) * (d - 1); ne.y += (ne.y - sw.y) * (d - 1); }
    void draw( ) override
    {
        put_line(sw,ne);
        put_line(point(sw.x,ne.y),point(ne.x,sw.y));
    }
};
list<shape*> shape::shapes;   // Размещение списка фигур

class Message{
    bool mess_arr[4]{};
public:
    void Leave_Message(int i){ mess_arr[i] = true;}
     void Give_Message(){
        for(int i = 0;i< sizeof(mess_arr)/sizeof(bool);++i){
         if (mess_arr[i])switch (i) {
         case 0:         std::cout<< "Unexpecteble error in drawing mode. Smth goes wrong (" << endl;
        std::cout<< "Please, correct your input data"<<endl;
        break;
         case 1:
             std::cout << "Some of the shapes you created has break the boarder,so they are not drawn" << endl;
                     std::cout << "Please check input data points in shapes constructor" << endl;
         break;
         case 2:
             std::cout << "Some of the figures are can't to rotate case of borders" << endl;
                     std::cout << "So,they won't..." << endl;
         break;
         case 3:
             std:: cout << "Some of the figures are can't to reflect case of borders" << endl;
                     std::cout << "So,they won't..." << endl;
             break;
         default:
             std::cout << "No errors found! Nice job!" << endl;
        }
     }
    }
};

bool Borders_Check_Error(shape *p,Message* M){
    bool flag = false;
    try {
        if(p->neast().x < XMAX && p->neast().y < YMAX && p->swest().x > -1  )
            p->draw();
        else throw CrossError();//ошибка выхода за экран
        //нужно отрисовать крест приемлемых габаритов посреди экрана если что-то пошло не так
    }
    catch (CrossError A){
        A.draw(); M->Leave_Message(1);flag = true;
    }
    catch (...) {
        screen_destroy( );
        M->Leave_Message(0);
        flag = true;
    }
    return flag;
}

void shape_refresh(Message* M) // Перерисовка ВСЕХ фигур на экране
{
    //Думаю, тут нужно писать ошибку выхода за границы экрана
    screen_clear( );
    bool flag = false;
    for (auto p : shape :: shapes) flag = Borders_Check_Error(p,M);//Динамическое связывание!!!
    if(!flag)screen_refresh( );
}

    class rotatable : virtual public shape { //Фигуры, пригодные к повороту
        bool is_rotate = false;
public:
        void Set_State(){is_rotate = !is_rotate;}
        bool Get_State() const { return is_rotate; }
        virtual void rotate_left( ) = 0;	//Повернуть влево
        virtual void rotate_right( ) = 0;	//Повернуть вправо
    };

void  Rotate_Check_Error(rotatable *p,char direct,Message *M){
    try {
        if(!p->Get_State()) switch (direct) {
                case 'l': p->rotate_left();
                case 'r': p->rotate_right();
                    break;
                default:
                    break;
            }//default : do nothing
        if(p->neast().x > XMAX || p->neast().y > YMAX || p->swest().x < -1 )
            throw CrossError() ;
    }
    catch (CrossError F) {F.draw(); p->Set_State(); M->Leave_Message(2);
    }
}

    class reflectable : virtual public shape { // Фигуры, пригодные
   bool reflected = false;
    public:					     // к зеркальному отражению
        void Set_State_R() {reflected = !reflected;}
        bool Get_State_R() const {return reflected;}
        virtual void flip_horisontally( ) = 0;	// Отразить горизонтально
        virtual void flip_vertically( ) = 0;	          // Отразить вертикально
    };

void Reflect_Check_Error(reflectable *p,char direct,Message *M ){
    try {
      // if (!p->Get_State_R())
        switch (direct) {
            case 'h': p->flip_horisontally();
                break;
            case 'v': p->flip_vertically();
                break;
            default:
                break;
        }
       if(p->neast().x > XMAX || p->neast().y > YMAX || p->swest().x < -1 )
            throw CrossError();
    }catch(CrossError F){ F.draw();  M->Leave_Message(3);
    }
}

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
            sw.x = ne.x - h * 2; ne.y = sw.y + w / 2; Set_State();}
        void rotate_left() // Поворот влево относительно sw
        { int w = ne.x - sw.x, h = ne.y - sw.y;
            ne.x = sw.x + h * 2; ne.y = sw.y + w / 2; Set_State();}
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