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

//    @Override
//    public boolean equals(Object o) {
//        if (this == o) return true;
//        if (o == null || getClass() != o.getClass()) return false;
//
//        Position position = (Position) o;
//
//        if (x != position.x) return false;
//        return y == position.y;
//    }
//
//    @Override
//    public int hashCode() {
//        int result = x;
//        result = 31 * result + y;
//        return result;
//    }
}
