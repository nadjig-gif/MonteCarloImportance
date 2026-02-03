#include <iostream>
#include <iomanip>
#include <random>
#include <functional>
#include <vector>
#include <cmath>
#include <numbers>
#include <memory> // For std::unique_ptr, eg std::unique_ptr<int>IntPTR = make_unique_ptr<int>(25) for *IntPTR = 25

// ==========================================
// ABSTRACT BASE CLASS
// ==========================================
template<typename T>
class Integrator {
public:
    // Pure virtual function.
    virtual double integrate(T& h, int n) = 0;

    // Virtual destructor is ESSENTIAL for polymorphism
    virtual ~Integrator() = default;
};

// ==========================================
// STRATEGY 1: CRUDE MONTE CARLO
// ==========================================
template<typename T>
class CrudeMonteCarlo : public Integrator<T> {

    std::mt19937 gen;

public:
    CrudeMonteCarlo():gen(std::random_device{}()){}; // we cant make gen a member directly.
    double integrate(T& h, int n) override
    {
        double Sum = 0;
        std::uniform_real_distribution<double> unif(0,1);
        for(int i = 0; i < n; i++)
        {
            double u = unif(gen);
            Sum += h(u);
        }
        return (Sum/n); // width is 1
    }
};

// ==========================================
// STRATEGY 2: IMPORTANCE SAMPLING
// ==========================================
// Where the function is close to 0, it contributes almost nothing to the integral (or sum of h(X's)).
// Sampling X's uniformly places as much value on h(X)~0 as a higher h(X) -- this is a waste of CPU processes.
// Importance Sampling: distribution is high where the function is high -- then correct for the over-representation
// the ratio u(x)/g(x), where u = uniform density, corrects for this.
// the integral ends up being 1/N * sum(h(X)/g(X)) through simplification.
template <typename T, typename G>
class ImportanceSampler : public Integrator<T> {
private:
    // The Probability Density Function of our proposal g(x)
    T g_pdf;
    
    // The Generator that produces samples distributed according to g(x)
    // CREATE THIS BY INVERSION.
    G g_generator;

public:
    // Constructor injection: The sampler doesn't decide g(x), the user does.
    ImportanceSampler(T& pdf, G& generator) 
        : g_pdf(pdf), g_generator(generator) {}

    
    double integrate(T& h, int n) override
    {   
        double integral = 0;
        for (int i = 0; i < n ; i++)
        {
            double x = g_generator();
            integral += (h(x) / g_pdf(x)) / n;
        }
        return integral;
    }
};
using T_func = std::function<double(double)>;
using G_func = std::function<double()>;

int main() {
    // 1. Define target h(x) = 4 * sqrt(1-x^2)
    std::function<double(double)> h = [](double x){
        return 4 * std::sqrt(1 - x*x);
    };
    
    // 2. Define N (e.g., 10,000)
    int N = 10000;

    // 3. Setup Random Engine (shared or local, your choice)
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    // pdf g
    std::function<double(double)> g_pdf = [](double x)
    {
        return 2*(1-x);
    };
    

    // 4. Instantiate CrudeMonteCarlo
    // make pointer to a CrudeMonteCarlo (derived Integrator):
    std::unique_ptr<Integrator<T_func>> crude_solver = 
    std::make_unique<CrudeMonteCarlo<T_func>>();
    // Find estimate and error
    double CrudeEstimate = crude_solver->integrate(h, N);
    double CrudeError = std::abs(CrudeEstimate - std::numbers::pi);
    
    // 5. Instantiate ImportanceSampler
    std::function<double()> g = [&rng, & dist]{
        double u = dist(rng);
        return 1 - std::sqrt( 1 - u); // apply the inverse CDF of g
    };
    
    std::unique_ptr<Integrator<T_func>> imp_solver = std::make_unique<ImportanceSampler<T_func, G_func>>(g_pdf, g);

    double ImportanceEstimate = imp_solver->integrate(h, N);
    double ImportanceError = std::abs(ImportanceEstimate - std::numbers::pi); 
    std::cout<<"\n\n";
    std::cout << std::left << std::setw(15)<< "Method  " <<"|" << std::setw(20)<< "Estimate "<<   "|" << "Error" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout <<  std::setw(15)<< "Crude  "  <<"|"<<std::setw(20)<< CrudeEstimate << "|" << CrudeError << std::endl;
    std::cout <<  std::setw(15)<< "Importance  "<<"|" << std::setw(20)<<ImportanceEstimate << "|" <<  ImportanceError << std::endl;


    return 0;
}