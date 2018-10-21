<?php

namespace hlt;

class GameMap
{
    /**
     * @var int
     */
    public $width;

    /**
     * @var int
     */
    public $height;

    /**
     * @var MapCell[][]
     */
    public $cells = [];

    public function __construct(int $width, int $height)
    {
        $this->width = $width;
        $this->height = $height;
    }

    public function atPosition(Position $position): MapCell
    {
        $normalized = $this->normalize($position);
        return $this->cells[$normalized->y][$normalized->x];
    }

    public function at(Entity $entity): MapCell
    {
        return $this->atPosition($entity->position);
    }

    public function calculateDistance(Position $source, Position $target): int
    {
        $normalizedSource = $this->normalize($source);
        $normalizedTarget = $this->normalize($target);

        $dx = abs($normalizedSource->x - $normalizedTarget->x);
        $dy = abs($normalizedSource->y - $normalizedTarget->y);

        $toroidal_dx = min($dx, $this->width - $dx);
        $toroidal_dy = min($dy, $this->height - $dy);

        return $toroidal_dx + $toroidal_dy;
    }

    public function normalize(Position $position): Position
    {
        $x = (($position->x % $this->width) + $this->width) % $this->width;
        $y = (($position->y % $this->height) + $this->height) % $this->height;
        return new Position($x, $y);
    }

    public function getUnsafeMoves( Position $source,  Position $destination)
    {
         $possibleMoves = [];

         $normalizedSource = $this->normalize($source);
         $normalizedDestination = $this->normalize($destination);

         $dx = abs($normalizedSource->x - $normalizedDestination->x);
         $dy = abs($normalizedSource->y - $normalizedDestination->y);
         $wrapped_dx = $this->width - $dx;
         $wrapped_dy = $this->height - $dy;

        if ($normalizedSource->x < $normalizedDestination->x) {
            $possibleMoves[] = $dx > $wrapped_dx ? new Direction(Direction::WEST) : new Direction(Direction::EAST);
        } else if ($normalizedSource->x > $normalizedDestination->x) {
            $possibleMoves[] = $dx < $wrapped_dx ? new Direction(Direction::WEST) : new Direction(Direction::EAST);
        }

        if ($normalizedSource->y < $normalizedDestination->y) {
            $possibleMoves[] = $dy > $wrapped_dy ? new Direction(Direction::NORTH) : new Direction(Direction::SOUTH);
        } else if ($normalizedSource->y > $normalizedDestination->y) {
            $possibleMoves[] = $dy < $wrapped_dy ? new Direction(Direction::NORTH) : new Direction(Direction::SOUTH);
        }

        return $possibleMoves;
    }

    public function naiveNavigate( Ship $ship,  Position $destination): Direction
    {
        // getUnsafeMoves normalizes for us
        foreach ( $this->getUnsafeMoves($ship->position, $destination) as $direction) {
             $targetPos = $ship->position->directionalOffset($direction);
            if (!$this->atPosition($targetPos)->isOccupied()) {
                $this->atPosition($targetPos)->markUnsafe($ship);
                return $direction;
            }
        }

        return new Direction(Direction::STILL);
    }

    public function _update(): void
    {
        for ($y = 0; $y < $this->height; ++$y) {
            for ($x = 0; $x < $this->width; ++$x) {
                $this->cells[$y][$x]->ship = null;
            }
        }

        $updateCount = Input::readInput()->getInt();

        for ($i = 0; $i < $updateCount; ++$i) {
            $input = Input::readInput();
            $x = $input->getInt();
            $y = $input->getInt();

            $this->cells[$y][$x]->halite = $input->getInt();
        }
    }

    static function _generate(): GameMap
    {
        $mapInput = Input::readInput();
        $width = $mapInput->getInt();
        $height = $mapInput->getInt();

        $map = new GameMap($width, $height);

        for ($y = 0; $y < $height; ++$y) {
            $rowInput = Input::readInput();

            for ($x = 0; $x < $width; ++$x) {
                $halite = $rowInput->getInt();
                $map->cells[$y][$x] = new MapCell(new Position($x, $y), $halite);
            }
        }

        return $map;
    }
}
