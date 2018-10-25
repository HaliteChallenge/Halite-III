<?php

namespace hlt;

class Player
{
    /**
     * @var int
     */
    public $id;

    /**
     * @var Shipyard
     */
    public $shipyard;

    /**
     * @var int
     */
    public $halite;
    /**
     * @var Ship[]
     */
    public $ships = [];
    /**
     * @var Dropoff[]
     */
    public $dropoffs = [];

    private function __construct(PlayerId $id, Shipyard $shipyard)
    {
        $this->id = $id;
        $this->shipyard = $shipyard;
    }

    function _update(int $numShips, int $numDropoffs, int $halite)
    {
        $this->halite = $halite;

        $this->ships = [];
        for ($i = 0; $i < $numShips; ++$i) {
            $ship = Ship::_generate($this->id);

            $this->ships[$ship->id->id] = $ship;
        }

        $this->dropoffs = [];
        for ($i = 0; $i < $numDropoffs; ++$i) {
            $dropoff = Dropoff::_generate($this->id);

            $this->dropoffs[$dropoff->id->id] = $dropoff;
        }
    }

    static function _generate(): Player
    {
        $input = Input::readInput();

        $playerId = new PlayerId($input->getInt());
        $shipyard_x = $input->getInt();
        $shipyard_y = $input->getInt();

        return new Player($playerId, new Shipyard($playerId, new Position($shipyard_x, $shipyard_y)));
    }
}
