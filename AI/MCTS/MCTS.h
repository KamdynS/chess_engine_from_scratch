#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include <memory>
#include <cmath>
#include <random>

class State {
public:
    virtual ~State() = default;
    virtual std::vector<std::unique_ptr<State>> getNextStates() const = 0;
    virtual bool isTerminal() const = 0;
    virtual double getReward() const = 0;
    virtual std::unique_ptr<State> clone() const = 0;
};

class Node {
public:
    Node(std::unique_ptr<State> state, Node* parent = nullptr);
    ~Node();

    void expand();
    Node* select();
    void backpropagate(double reward);
    bool isFullyExpanded() const;
    bool isLeaf() const;

    std::unique_ptr<State> state;
    Node* parent;
    std::vector<std::unique_ptr<Node>> children;
    double totalReward;
    int visits;
};

class MCTS {
public:
    MCTS(int iterations, double explorationParameter = std::sqrt(2));
    Node* search(std::unique_ptr<State> initialState);

private:
    int iterations;
    double explorationParameter;
    std::mt19937 rng;
};

#endif 