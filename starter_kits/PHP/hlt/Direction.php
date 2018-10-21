<?php

namespace hlt;

class Direction
{
    public const NORTH = 'n';
    public const EAST = 'e';
    public const SOUTH = 's';
    public const WEST = 'w';
    public const STILL = 'o';

    public const ALL_CARDINALS = [
        self::NORTH,
        self::EAST,
        self::SOUTH,
        self::WEST,
        self::STILL,
    ];

    /**
     * @var string
     */
    private $charValue;

    public static function getRandomDirection(): Direction
    {
        return new Direction(self::ALL_CARDINALS[array_rand(self::ALL_CARDINALS)]);
    }

    public function __construct(string $charValue)
    {
        $this->charValue = $charValue;
    }

    /**
     * @return Direction
     */
    public function invertDirection(): Direction
    {
        switch ($this->charValue) {
            case self::NORTH:
                return new Direction(self::SOUTH);
            case self::EAST:
                return new Direction(self::WEST);
            case self::SOUTH:
                return new Direction(self::NORTH);
            case self::WEST:
                return new Direction(self::EAST);
            case self::STILL:
                return new Direction(self::STILL);
            default:
                throw new \RuntimeException('Unknown direction ' . $this);
        }
    }

    /**
     * @return string
     */
    public function getCharValue(): string
    {
        return $this->charValue;
    }

    public function equals($other): bool
    {
        if (!($other instanceof self)) {
            return false;
        }

        return $other->getCharValue() === $this->getCharValue();
    }

    public function __toString()
    {
        return $this->charValue;
    }
}
