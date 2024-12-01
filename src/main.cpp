//WERSJA NR2. Z WIRT. POLIMOROFIZMEM
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <RandomNameGenerator.hpp>
//#include "RandomNameGenerator.hpp"

class Pracownik
{
protected:
    std::string imie;
    double wynagrodzenie;
    
public:
    Pracownik(double _wynagrodzenie) : imie(getRandomName()), wynagrodzenie(_wynagrodzenie) {}
    virtual ~Pracownik() {}
    virtual void print() const { std::cout << "Imie: " << imie << std::endl; }
    double       get_wynagrodzenie() const { return wynagrodzenie; }
};

class Inz : public Pracownik
{
    std::string wydzial;

public:
    static const double CI;
    Inz(const std::string& _wydzial) : Pracownik(1000.0), wydzial(_wydzial) {}
    void print() const override
    {
        Pracownik::print();
        std::cout << "Wydzial: " << wydzial << std::endl;
    }
};

class Mag : public Pracownik
{
    bool obsl_widl;

public:
    static const double CMag;
    Mag(bool _obsl_widl) : Pracownik(700.0), obsl_widl(_obsl_widl) {}
    void print() const override
    {
        Pracownik::print();
        std::cout << "Obsluguje wozek widlowy: " << (obsl_widl ? "Tak" : "Nie") << std::endl;
    }
};

class Mkt : public Pracownik
{
    int follows;

public:
    static const double CMkt;
    Mkt(int _follows) : Pracownik(500.0), follows(_follows) {}
    void print() const override
    {
        Pracownik::print();
        std::cout << "Obserwujacy: " << follows << std::endl;
    }
};

class Rob : public Pracownik
{
    double but;

public:
    static const double CR;
    Rob(double _but) : Pracownik(600.0), but(_but) {}
    void print() const override
    {
        Pracownik::print();
        std::cout << "Rozmiar buta: " << but << std::endl;
    }
};
const double Inz::CI   = 50.0;//stale wydajnosci
const double Mag::CMag = 100;  
const double Mkt::CMkt = 20.0; 
const double Rob::CR   = 20.0; 

class Kredyt
{
    double dlug;
    int    pozostale_raty;
    double odsetki;

public:
    static const int    MAX_CZAS_SPLATY = 24;
    static const double STOPA_ODSETEK;        

    Kredyt(double _dlug, int _pozostale_raty)
    {
        if (_pozostale_raty > MAX_CZAS_SPLATY)
        {
            std::cerr << "Czas splaty nie moze przekraczac " << MAX_CZAS_SPLATY << " miesiêcy!" << std::endl;
            throw std::invalid_argument("Przekroczony maksymalny czas splaty");
        }
        dlug           = _dlug;
        pozostale_raty = _pozostale_raty;
        odsetki        = _dlug * STOPA_ODSETEK * _pozostale_raty;
        dlug += odsetki;                                          
    }

    double splac_rate()
    {
        if (pozostale_raty <= 0)
        {
            std::cerr << "Kredyt zostal ju¿ w pelni splacony!" << std::endl;
            return 0.0;
        }
        double rata = dlug / pozostale_raty;//aktualna rata
        pozostale_raty--;
        dlug -= rata;
        return rata;
    }

    bool czy_splacony() const { return pozostale_raty <= 0; }
    double get_dlug() const { return dlug; }
};

const double Kredyt::STOPA_ODSETEK = 0.01;

class Firma
{
    double                    stan_konta;
    std::vector< Pracownik* > pracownicy;
    std::vector< Kredyt >     kredyty;

    static const int    N = 12;//ostatnie 12 mies.
    static const double M;//limit zadluzenia
    double              historia_przychodow[N];
    int                 indeks_przychodu;

public:
    Firma() : stan_konta(10000), indeks_przychodu(0)
    {
        pracownicy.push_back(new Inz("Mechaniczny"));// poczaatkowa kadra pracownicza po 1 z kazdego rodzaju
        pracownicy.push_back(new Mag(true));
        pracownicy.push_back(new Mkt(500));
        pracownicy.push_back(new Rob(42.5));
        for (int i = 0; i < N; i++)
        {
            historia_przychodow[i] = 500.0;
        }
    }

    ~Firma()
    {
        for (auto p : pracownicy)
            delete p;
    }

    void zatrudnij(Pracownik* p) { pracownicy.push_back(p); }

    double oblicz_pojemnosc_magazynu() const
    {
        int liczba_magazynierow = 0;
        for (const auto& p : pracownicy)
        {
            if (dynamic_cast< const Mag* >(p))
            {
                liczba_magazynierow++;
            }
        }
        return liczba_magazynierow * Mag::CMag;
    }

    double oblicz_cene_produktu() const
    {
        int liczba_inzynierow = 0;
        for (const auto& p : pracownicy)
        {
            if (dynamic_cast< const Inz* >(p))
            {
                liczba_inzynierow++;
            }
        }
        return liczba_inzynierow * Inz::CI;
    }

    double oblicz_popyt() const
    {
        int liczba_marketerow = 0;
        for (const auto& p : pracownicy)
        {
            if (dynamic_cast< const Mkt* >(p))
            {
                liczba_marketerow++;
            }
        }
        return liczba_marketerow * Mkt::CMkt;
    }

    double oblicz_teoretyczna_produkcje() const
    {
        int liczba_robotnikow = 0;
        for (const auto& p : pracownicy)
        {
            if (dynamic_cast< const Rob* >(p))
            {
                liczba_robotnikow++;
            }
        }
        return liczba_robotnikow * Rob::CR;
    }

    double oblicz_przychod()
    {
        double pojemnosc_magazynu    = oblicz_pojemnosc_magazynu();
        double teoretyczna_produkcja = oblicz_teoretyczna_produkcje();
        double faktyczna_produkcja   = std::min(pojemnosc_magazynu, teoretyczna_produkcja);
        double popyt              = oblicz_popyt();
        double liczba_sprzedanych = std::min(faktyczna_produkcja, popyt);
        double cena_produktu = oblicz_cene_produktu();
        double przychod      = liczba_sprzedanych * cena_produktu;
        stan_konta += przychod;
        historia_przychodow[indeks_przychodu] = przychod;
        indeks_przychodu                      = (indeks_przychodu + 1) % N;
        return przychod;
    }
    double oblicz_zadluzenie() const
    {
        double suma = 0.0;
        for (const auto& kredyt : kredyty)
        {
            suma += kredyt.get_dlug();
        }
        return suma;
    }
    double oblicz_wartosc_spolki() const
    {
        double suma = 0.0;
        for (int i = 0; i < N; i++)
        {
            suma += historia_przychodow[i];
        }
        return suma / N; //przychod z ostatnich N miesiecy usredniony
    }

    void wez_kredyt(double kwota, int czas_splaty)
    {
        const int MAX_CZAS_SPLATY = 24;
        if (czas_splaty > MAX_CZAS_SPLATY)
        {
            std::cout << "Czas splaty nie mo¿e przekraczac " << MAX_CZAS_SPLATY << " miesiecy!" << std::endl;
            return;
        }
        double maks_zadluzenie   = M * oblicz_wartosc_spolki();
        double obecne_zadluzenie = oblicz_zadluzenie();
        if (obecne_zadluzenie + kwota > maks_zadluzenie)
        {
            std::cout << "Nie mozna zaciagnac kredytu!" << std::endl;
            std::cout << "Obecne zadluzenie: " << obecne_zadluzenie << ", Maksymalne zadluzenie: " << maks_zadluzenie
                      << std::endl;
            return;
        }
        //daj kredyt
        kredyty.emplace_back(kwota, czas_splaty);
        stan_konta += kwota;
        std::cout << "Zaciagnieto kredyt na kwote: " << kwota << ", czas splaty: " << czas_splaty << " miesiecy."
                  << std::endl;
    }

     void drukuj_stan_firmy() const
    {
        std::cout << "Stan konta: " << stan_konta << std::endl;
        std::cout << "Pojemnosc magazynu: " << oblicz_pojemnosc_magazynu() << std::endl;
        std::cout << "Cena produktu: " << oblicz_cene_produktu() << std::endl;
        std::cout << "Popyt: " << oblicz_popyt() << std::endl;
    }

    void zaplac_wynagrodzenie()
    {
        double koszt = 0.0;
        for (const auto& p : pracownicy)
        {
            koszt += p->get_wynagrodzenie(); //wynagrodzennie pracownikow
        }
        stan_konta -= koszt;
        std::cout << "Koszt wynagrodzen: " << koszt << std::endl;
    }

    void splac_raty()
    {
        double laczna_rata = 0;
        for (auto& k : kredyty)
        {
            if (!k.czy_splacony())
                laczna_rata += k.splac_rate();
        }
        stan_konta -= laczna_rata;
    }

    double get_stan_konta() const { return stan_konta; }

    void drukuj_pracownikow() const
    {
        for (const auto& p : pracownicy)
        {
            p->print();
            std::cout << "------------" << std::endl;
        }
    }
};
const double Firma::M = 3.0; //stala limitu zadluzenia

class Gra
{
    bool         stan;
    Firma*       firma;
    const double CEL_WARTOSCI_SPOLKI = 10000.0;

public:
    Gra() : stan(true), firma(new Firma())
    {
        srand(static_cast< unsigned >(time(0)));
    }
    ~Gra() { delete firma; }

    void akcja_gracza()
    {
        std::string komenda;
        std::cout << "Podaj komende: ";
        std::cin >> komenda;

        if (komenda == "lp")
        {
            firma->drukuj_pracownikow();
        }
        else if (komenda == "zinz")
        {
            firma->zatrudnij(new Inz("Mechaniczny"));
        }
        else if (komenda == "zmag")
        {
            firma->zatrudnij(new Mag(true));
        }
        else if (komenda == "zmkt")
        {
            firma->zatrudnij(new Mkt(500));
        }
        else if (komenda == "zrob")
        {
            firma->zatrudnij(new Rob(42.5));
        }
        else if (komenda == "kred")
        {
            double kwota;
            int    czas;
            std::cout << "Podaj kwote kredytu: ";
            std::cin >> kwota;
            std::cout << "Podaj czas splaty: ";
            std::cin >> czas;
            firma->wez_kredyt(kwota, czas);
        }
        else if (komenda == "kt")
        {
            double przychod = firma->oblicz_przychod();
            std::cout << "Przychod w tym miesiacu: " << przychod << std::endl;
            firma->zaplac_wynagrodzenie();
            firma->splac_raty();
            firma->drukuj_stan_firmy();
            sprawdz_stan_gry();
        }
        else
        {
            std::cout << "Nieznana komenda!" << std::endl;
        }
    }

    void tick()
    {
        while (stan)
        {
            akcja_gracza();
        }
    }
    void sprawdz_stan_gry()
    {
        double wartosc_spolki = firma->oblicz_wartosc_spolki(); // wartosc spolki - zadluzenie = netto
        double zadluzenie     = firma->oblicz_zadluzenie();
        double netto_wartosc  = wartosc_spolki - zadluzenie;
        if (netto_wartosc > CEL_WARTOSCI_SPOLKI)
        {
            std::cout << " Wygrana. Wartosc spolki netto wynosi: " << netto_wartosc << std::endl;
            stan = false;
            return;
        }
        if (firma->get_stan_konta() < 0)
        {
            std::cout << "Firma nie ma pieniedzy na koncie. Przegrales." << std::endl;
            stan = false;
            return;
        }
    }
};

int main()
{
    Gra gra;
    gra.tick();
    return 0;
}