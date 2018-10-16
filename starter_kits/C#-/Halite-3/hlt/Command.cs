using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace MyBot.hlt
{
    class Command
    {
        public string command;

        public static Command spawnShip()
        {
            return new Command("g");
        }

        public static Command transformShipIntoDropoffSite(EntityId id)
        {
            return new Command("c " + id);
        }

        public static Command move(EntityId id, Direction direction)
        {
            return new Command("m " + id + ' ' + direction.charValue);
        }

        private Command(string command)
        {
            this.command = command;
        }

        public override bool Equals(Object o)
        {
            if (this == o) return true;
            //if (o == null || getClass() != o.getClass()) return false;

            Command command1 = (Command)o;

            return command.Equals(command1.command);
        }

        public override int GetHashCode()
        {
            return command.GetHashCode();
        }
    }
}
