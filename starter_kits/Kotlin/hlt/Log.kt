package hlt

import java.io.FileWriter
import java.io.IOException
import java.util.*

class Log private constructor(private val file: FileWriter) {

    private class AtExit : Thread() {
        override fun run() {
            if (INSTANCE != null) {
                return
            }

            val now_in_nanos = System.nanoTime()
            val filename = "bot-unknown-$now_in_nanos.log"
            try {
                FileWriter(filename).use { writer ->
                    for (message in LOG_BUFFER) {
                        writer.append(message).append('\n')
                    }
                }
            } catch (e: IOException) {
                // Nothing much we can do here.
            }

        }
    }

    companion object {

        private var INSTANCE: Log? = null
        private val LOG_BUFFER = ArrayList<String>()

        init {
            Runtime.getRuntime().addShutdownHook(AtExit())
        }

        internal fun open(botId: Int) {
            if (INSTANCE != null) {
                Log.log("Error: log: tried to open($botId) but we have already opened before.")
                throw IllegalStateException()
            }

            val filename = "bot-$botId.log"
            val writer: FileWriter
            try {
                writer = FileWriter(filename)
            } catch (e: IOException) {
                throw IllegalStateException(e)
            }

            INSTANCE = Log(writer)

            try {
                for (message in LOG_BUFFER) {
                    writer.append(message).append('\n')
                }
            } catch (e: IOException) {
                throw IllegalStateException(e)
            }

            LOG_BUFFER.clear()
        }

        fun log(message: String) {
            if (INSTANCE == null) {
                LOG_BUFFER.add(message)
                return
            }

            try {
                INSTANCE!!.file.append(message).append('\n').flush()
            } catch (e: IOException) {
                e.printStackTrace()
            }

        }
    }
}
