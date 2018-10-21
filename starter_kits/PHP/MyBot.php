<?php

use hlt\Constants;
use hlt\Direction;
use hlt\Game;

spl_autoload_register(function ($class) {
    $prefix = 'hlt\\';
    $base_dir = __DIR__ . '/hlt/';
    $len = strlen($prefix);
    if (strncmp($prefix, $class, $len) !== 0) {
        return;
    }
    $relative_class = substr($class, $len);
    $file = $base_dir . str_replace('\\', '/', $relative_class) . '.php';
    if (file_exists($file)) {
        require $file;
    }
});

$game = new Game();
// At this point "game" variable is populated with initial map data.
// This is a good place to do computationally expensive start-up pre-processing.
// As soon as you call "ready" function below, the 2 second per turn timer will start.
$game->ready('MyPHPBot');

for (;;) {
    $game->updateFrame();
    $me = $game->me;
    $gameMap = $game->gameMap;

    $commandQueue = [];

    foreach ($me->ships as $ship) {
        if ($gameMap->at($ship)->halite < Constants::$MAX_HALITE / 10 || $ship->isFull()) {
            $commandQueue[] = $ship->move(Direction::getRandomDirection());
        } else {
            $commandQueue[] = $ship->stayStill();
        }
    }

    if (
        $game->turnNumber <= 200 &&
        $me->halite >= Constants::$SHIP_COST &&
        !$gameMap->at($me->shipyard)->isOccupied()) {
        $commandQueue[] = $me->shipyard->spawn();
    }

    $game->endTurn($commandQueue);
}
