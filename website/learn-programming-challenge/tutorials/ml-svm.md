---
layout: doc_page
title: Supervised Learning Bot
breadcrumb: Learn
toc: false
description: Learn about using an SVM (Support Vector Machine) to train a bot
#sort_key: 2
---


<div class="doc-section" markdown="1">

#### Introduction
Training a bot for an AI competition like Halite III makes a lot of sense - especially if you have performant bots written by humans that can generate data for your training. We tried using SVM to help bots navigate to halite-rich areas on the map.

<br/>
#### What is an SVM?
A Support Vector Machine (SVM) is a supervised machine learning model with associated learning algorithms for classification and regression analysis. SVMs are based on the idea of finding a hyperplane<sup>1</sup> that best divides marked data into two groups, and in doing so building a model that can assign new unmarked data to one group or the other. A more in-depth look at SVMs is available [here](http://scikit-learn.org/stable/modules/svm.html).

We used a specific kind of SVM called an rbf kernel weighted classifier. RBF kernel works well in practice, and is relatively easy to calibrate. It’s well-studied and widely understood, and makes a great starting place.

<br/>
#### Our Data and Method
A good strategy for developing an ML bot is to train on a winning bot’s data. To generate training data for two ML bots, we developed two simple bots with different navigation styles. One of these simple bots was more aggressive and the other was more passive. We ran four games for each of the bots, and saved every game state frame for training, resulting in 1,000 frames of data. This data was parsed and trimmed; we removed 25% randomly to improve training time. We centered the game action, and rotated each vector to improve overfitting and give the model better and more balanced decision-making abilities. This resulted in two models, an aggressive navigation model that we expected to explore boldly, and a passive navigation model that we expected to stay closer to base, move less often, and return more frequently.

<br/>
#### The Bots
We built a simple bot that uses the machine-taught models to navigate. We used an object oriented style that allowed us to share code between the passive and aggressive bots; SVM bot is an object initialized by its training weights, which come from whichever model it is given at runtime.

We found that the machine-taught models were bad at getting ships reliably back to base, so we coded that capability manually. We were thus additional able to further trim the data, throwing out any data about moving the ship closer to the shipyard. We also manually coded the logic to construct new ships, as this strategic decision was not a focus of our machine learning. After this data trimming, the bots took a quick ten minutes each to train.

Overall, these two bots are less successful at playing Halite III than the bots used to generate the training data. The ML bots decouple navigation from strategy, which makes them worse. The ML bots mimic moves without considering an overall strategic plan. A more advanced starting bot and more in-depth training might produce better results.

<br/>
<small><sup>1</sup> “In geometry, a hyperplane is a subspace whose dimension is one less than that of its ambient space. If a space is 3-dimensional then its hyperplanes are the 2-dimensional planes, while if the space is 2-dimensional, its hyperplanes are the 1-dimensional lines,” via Wikipedia.</small>
</div>
