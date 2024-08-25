#include "mcts.h"
#include <algorithm>

Node::Node(std::unique_ptr<State> state, Node* parent)
    : state(std::move(state)), parent(parent), totalReward(0), visits(0) {}

Node::~Node() {
    for (auto& child : children) {
        child.reset();
    }
}

void Node::expand() {
    auto nextStates = state->getNextStates();
    for (auto& nextState : nextStates) {
        children.push_back(std::make_unique<Node>(std::move(nextState), this));
    }
}

Node* Node::select() {
    if (!isFullyExpanded()) {
        return this;
    }
    return std::max_element(children.begin(), children.end(),
        [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
            return a->totalReward / a->visits < b->totalReward / b->visits;
        })->get();
}

void Node::backpropagate(double reward) {
    visits++;
    totalReward += reward;
    if (parent) {
        parent->backpropagate(reward);
    }
}

bool Node::isFullyExpanded() const {
    return !children.empty() && children.size() == state->getNextStates().size();
}

bool Node::isLeaf() const {
    return children.empty();
}

MCTS::MCTS(int iterations, double explorationParameter)
    : iterations(iterations), explorationParameter(explorationParameter), rng(std::random_device{}()) {}

Node* MCTS::search(std::unique_ptr<State> initialState) {
    auto root = std::make_unique<Node>(std::move(initialState));

    for (int i = 0; i < iterations; ++i) {
        Node* node = root.get();

        // Selection
        while (!node->isLeaf() && node->isFullyExpanded()) {
            node = node->select();
        }

        // Expansion
        if (!node->isFullyExpanded()) {
            node->expand();
            node = node->children.back().get();
        }

        // Simulation
        auto currentState = node->state->getNextStates()[0]->clone();
        while (!currentState->isTerminal()) {
            auto nextStates = currentState->getNextStates();
            std::uniform_int_distribution<> dist(0, nextStates.size() - 1);
            currentState = nextStates[dist(rng)]->clone();
        }

        // Backpropagation
        node->backpropagate(currentState->getReward());
    }

    return root->select();
}