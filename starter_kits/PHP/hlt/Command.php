<?php

namespace hlt;

class Command
{
    /**
     * @var string
     */
    public $command;

    public static function spawnShip(): Command
    {
        return new Command('g');
    }

    public static function transformShipIntoDropoffSite(EntityId $id): Command
    {
        return new Command('c ' . $id);
    }

    public static function move(EntityId $id, Direction $direction): Command
    {
        return new Command('m ' . $id . ' ' . $direction->getCharValue());
    }

    private function __construct(string $command)
    {
        $this->command = $command;
    }

    public function equals($o): bool
    {
        if (!($o instanceof self)) {
            return false;
        }

        return $this->command === $o->command;
    }
}
