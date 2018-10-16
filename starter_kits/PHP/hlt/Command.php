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

//    @Override
//    public boolean equals(Object o) {
//        if (this == o) return true;
//        if (o == null || getClass() != o.getClass()) return false;
//
//        Command command1 = (Command) o;
//
//        return command.equals(command1.command);
//    }
//
//    @Override
//    public int hashCode() {
//        return command.hashCode();
//    }
}
