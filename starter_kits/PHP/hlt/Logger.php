<?php

namespace hlt;

class Logger
{

    public static $LOGGER = null;

    /**
     * @var string
     */
    private $filename;

    /**
     * Logger constructor.
     * @param int $botId
     */
    public function __construct(int $botId = 0)
    {
        $this->filename = 'bot-' . $botId . '.log';
    }

    public static function open(int $botId): void
    {
        if (self::$LOGGER === null) {
            self::$LOGGER = new Logger($botId);
        }
    }

    public static function log(string $string): void
    {
        if (self::$LOGGER === null) {
            self::$LOGGER = new Logger();
        }

        self::$LOGGER->logToFile($string);
    }

    public function logToFile(string $string)
    {
        file_put_contents($this->filename, $string . "\n", FILE_APPEND);
    }

}