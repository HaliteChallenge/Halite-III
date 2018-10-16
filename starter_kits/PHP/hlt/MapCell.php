<?php

namespace hlt;

class MapCell
{
    /**
     * @var Position
     */
    public $position;
    /**
     * @var int
     */
    public $halite;
    /**
     * @var Ship
     */
    public $ship;
    /**
     * @var Entity
     */
    public $structure;

    public function __construct(Position $position, int $halite)
    {
        $this->position = $position;
        $this->halite = $halite;
    }

    public function isEmpty(): bool
    {
        return $this->ship === null && $this->structure === null;
    }

    public function isOccupied(): bool
    {
        return $this->ship !== null;
    }

    public function hasStructure(): bool
    {
        return $this->structure !== null;
    }

    public function markUnsafe(Ship $ship): void
    {
        $this->ship = $ship;
    }
}
