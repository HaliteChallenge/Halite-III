<?php

namespace hlt;

class Game
{
    /**
     * @var int
     */
    public $turnNumber;
    /**
     * @var PlayerId
     */
    public $myId;
    /**
     * ArrayList<Player>
     */
    public $players = [];
    /**
     * @var Player
     */
    public $me;
    /**
     * @var GameMap
     */
    public $gameMap;

    public function __construct()
    {
        Constants::populateConstants(Input::readLine());

        $input = Input::readInput();
        $numPlayers = $input->getInt();

        $myId = new PlayerId($input->getInt());

        Logger::open($myId->id);

        for ($i = 0; $i < $numPlayers; ++$i) {
            $player = Player::_generate();
            $this->players[$player->id->id] = $player;
        }

        $this->me = $this->players[$myId->id];
        $this->gameMap = GameMap::_generate();
    }

    public function ready(string $name)
    {
        echo $name . "\n";
    }

    public function updateFrame(): void
    {
        $this->turnNumber = Input::readInput()->getInt();
        Logger::log('=============== TURN ' . $this->turnNumber . ' ================');

        for ($i = 0, $iMax = \count($this->players); $i < $iMax; ++$i) {
            $input = Input::readInput();

            $currentPlayerId = new PlayerId($input->getInt());

            $numShips = $input->getInt();
            $numDropoffs = $input->getInt();
            $halite = $input->getInt();

            Logger::log('current player  ' . $currentPlayerId->id);

            $this->players[$currentPlayerId->id]->_update($numShips, $numDropoffs, $halite);
        }

        $this->gameMap->_update();

        /** @var Player $player */
        foreach ($this->players as $player) {
            foreach ($player->ships as $ship) {
                $this->gameMap->at($ship)->markUnsafe($ship);
            }

            $this->gameMap->at($player->shipyard)->structure = $player->shipyard;

            foreach ($player->dropoffs as $dropoff) {
                $this->gameMap->at($dropoff)->structure = $dropoff;
            }
        }
    }

    public function endTurn(array $commands): void
    {
        foreach ($commands as $command) {
            echo $command->command . ' ';
        }
        echo "\n";
    }
}
