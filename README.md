# MCM_Importance.cpp: High-Performance Monte Carlo Integration

This C++ implementation demonstrates an extensible framework for estimating deterministic integrals using advanced **Stochastic Simulation** techniques. The project serves as a showcase for architectural design patterns and variance reduction strategies in numerical computing.

## Technical Architecture

The system utilizes a **Strategy Pattern** facilitated by C++ templates and inheritance to provide a decoupled, high-performance integration engine.

### Core Components

* **Abstract Interface (`Integrator<T>`)**: A template-based base class that enforces a common interface for different integration strategies. It uses **Pure Virtual Functions** to ensure that any derived solver implements the core `integrate` logic.
* **Polymorphic Execution**: The use of `std::unique_ptr<Integrator<T>>` demonstrates modern **RAII (Resource Acquisition Is Initialization)** principles and allows for runtime switching between algorithms while maintaining strict memory safety.
* **Functional Abstraction**: Leverages `std::function` and **Lambda Expressions** to pass mathematical models as first-class objects, allowing the integrator to remain agnostic of the specific function being evaluated.

---

## Numerical Methodologies

The program evaluates the integral of  over the interval  to estimate the value of .

### 1. Crude Monte Carlo (`CrudeMonteCarlo`)

This baseline strategy uses a uniform distribution  to sample the domain.

* **Logic**: It treats every sub-region of the integral with equal weight.
* **Efficiency**: Suffers from high variance, especially in regions where the function fluctuates significantly.

### 2. Importance Sampling (`ImportanceSampler`)

An advanced variance reduction technique that optimizes CPU cycles by focusing on "important" regions of the function.

* **The Problem**: In Crude MC, regions where  are sampled as often as regions where  is large, leading to statistical noise.
* **The Solution**: We introduce a **Proposal Density**  that roughly follows the shape of .
* **Inverse Transform Sampling**: To sample from , we apply the **Inversion Method** to a uniform variable :


* **Weight Correction**: The estimator is corrected by the likelihood ratio  to ensure an unbiased result.

---

## Performance Metrics

Results for  iterations:

| Strategy | Logic | Error Propensity |
| --- | --- | --- |
| **Crude** | Uniform Sampling | High |
| **Importance** | Biased Sampling + Weighting | **Low** |


## How to Compile

The source file **MCM_Importance.cpp** requires a C++20 compliant compiler.

```bash
# Using GCC
g++ -std=c++20 MCM_Importance.cpp -o MCM_Importance

# Run the simulation
./MCM_Importance

```

