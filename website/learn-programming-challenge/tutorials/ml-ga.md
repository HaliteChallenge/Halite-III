---
layout: doc_page
title: Genetic Algorithm Bot
breadcrumb: Learn
toc: false
description: Learn about using a genetic algorithm to tune a bot.
#sort_key: 2
---


<div class="doc-section" markdown="1">

#### Introduction
Using advanced algorithms to iterate quickly on the design of a bot is a strategy for Halite III. Using a genetic algorithm, we tried varying the parameters of a basic bot and testing the resulting permutations to discover the strongest bot.

<br/>
#### What is a Genetic Algorithm?
A genetic algorithm is a process for finding an optimized solution to a problem. Genetic algorithms are inspired by biology, and involve creating many copies of a potential solution with slight variations, testing them with a fitness function, and choosing the most effective solutions to create the new generation. This process repeats either until a maximum number of generations has been produced, or when a solution is found that satisfies the minimum criteria.

<br/>
#### Our Data and Method
We used a standard genetic algorithm implementation. Our genetic algorithm allowed eight solutions in each generation, and ten generations total. We began with a simple bot that builds ships, and tells its ships to move toward the highest halite it can see, collect halite, and return to the shipyard. This basic code is a template ready to accept our varying parameters.

The genetic algorithm varied five parameters to produce different solutions:
* Maximum distance the ship “sees” when looking for halite-rich cells (1 to 32)
* Maximum halite at which a ship moves off a tile (0 to 1,000)
* Maximum halite at which a ship returns to a shipyard or dropoff (0 to 1,000)
* Number of ships to produce in a game (1 to 100)
* The point in the game when the bot spawns no more ships (turn number 1 to 500)

The method for varying the parameters in the first generation was to choose a random integer between each parameter’s minimum and maximum using Python’s inbuilt <code>random.randint()</code> function. After that, the parameters of successful “parents” were mutated by a normal distribution of 10% of the total range.

The fitness function plays the bots in a generation against each other. Each bot generated is ranked by the results of four games, and the upper half of the bots form the next generation. Once we have a set of winning parent bots, we build the next generation in two ways. We mutate some parents to produce new variations, and we also cross parents in a more “genetic” way to combine their characteristics. Finally, we look for duplicates among the new generation and mutate any copies to keep variation high.


<br/>
#### The Bot
The bot developed with our genetic algorithm becomes more performant through the process. Though our bot certainly improved, it is likely only competitive with other bots that have completely random parameter values. Genetic algorithms are best for parameter tuning a Halite III bot, rather than training a bot from scratch.

Adding parameters to increase the diversity could produce better bots, particularly if balanced with more generations or more candidate solutions per generation. Adding parameters for strategies like building dropoffs and engaging other players could add to the sophistication of the final bot.

Having an time-optimized bot before handing it to a genetic algorithm is useful, as a bot that takes a long time to play a match of Halite will directly increase the training time. As you add parameters and detailed strategy, keep in mind that your genetic parameter tuning could end up being a long process.

<br/>
</div>
