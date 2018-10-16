namespace Halite3.Core.Utils
{
    public static class ExtensionMethods
    {
        public static int Mod(this int number, int mod)
        {
            return (number % mod + mod) % mod;
        }
    }
}
