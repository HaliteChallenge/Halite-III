<?php

namespace hlt;

class Position
{
    /**
     * @var  int
     */
    public $x;

    /**
     * @var  int
     */
    public $y;

    public function __construct(int $x, int $y)
    {
        $this->x = $x;
        $this->y = $y;
    }

    public function directionalOffset(Direction $d): Position
    {
        switch ($d) {
            case Direction::NORTH:
                $dx = 0;
                $dy = -1;
                break;
            case Direction::SOUTH:
                $dx = 0;
                $dy = 1;
                break;
            case Direction::EAST:
                $dx = 1;
                $dy = 0;
                break;
            case Direction::WEST:
                $dx = -1;
                $dy = 0;
                break;
            case Direction::STILL:
                $dx = 0;
                $dy = 0;
                break;
            default:
                throw new \RuntimeException('Unknown direction ' . $d);
        }

        return new Position($this->x + $dx, $this->y + $dy);
    }

    public function equals($other): bool
    {
        if (!($other instanceof self)) {
            return false;
        }

        return $this->x === $other->x && $this->y === $other->y;
    }
}
