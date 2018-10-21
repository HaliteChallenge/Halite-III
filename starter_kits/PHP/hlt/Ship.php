<?php

namespace hlt;

class Ship extends Entity
{
    /**
     * @var int
     */
    public $halite;

    public function __construct(PlayerId $owner, EntityId $id, Position $position, int $halite)
    {
        parent::__construct($owner, $id, $position);
        $this->halite = $halite;
    }

    public function isFull(): bool
    {
        return $this->halite >= Constants::$MAX_HALITE;
    }

    public function makeDropoff(): Command
    {
        return Command::transformShipIntoDropoffSite($this->id);
    }

    public function move(Direction $direction): Command
    {
        return Command::move($this->id, $direction);
    }

    public function stayStill(): Command
    {
        return Command::move($this->id, new Direction(Direction::STILL));
    }

    static function _generate(PlayerId $playerId): Ship
    {
        $input = Input::readInput();
        $shipId = new EntityId($input->getInt());

        $x = $input->getInt();
        $y = $input->getInt();
        $halite = $input->getInt();

        return new Ship($playerId, $shipId, new Position($x, $y), $halite);
    }

    public function equals($other): bool
    {
        if (!($other instanceof self)) {
            return false;
        }

        return $this->halite === $other->halite;
    }
}
