package hlt

import java.io.{FileWriter, IOException}

import scala.collection.mutable

object Log {
  private val _buffer = new mutable.Queue[String]
  private var _logFileWriter: FileWriter = null

  def open(botId: Int) = {
    if (_logFileWriter != null) {
      Log.log(s"Error: log: tried to open($botId) but we have already opened before.")
      throw new IllegalStateException
    }
    val filename = s"bot-$botId.log"
    try {
      _logFileWriter = new FileWriter(filename)
      _buffer.foreach(line => _logFileWriter.append(line).append('\n'))
      _buffer.clear()
    } catch {
      case e: IOException =>
        throw new IllegalStateException(e)
    }
  }

  def log(message: String) = {
    if (_logFileWriter == null) {
      _buffer.enqueue(message)
    }
    else {
      try _logFileWriter.append(message).append('\n').flush()
      catch {
        case e: IOException =>
          e.printStackTrace()
      }
    }
  }

  private class AtExit extends Thread {
    override def run(): Unit = {
      if (_logFileWriter != null) return
      val now_in_nanos = System.nanoTime
      val filename = s"bot-unknown-$now_in_nanos.log"
      var writer: FileWriter = null;
      try {
        writer = new FileWriter(filename)
        _buffer.foreach(line => _logFileWriter.append(line).append('\n'))
      } catch {
        case e: IOException =>
        // Nothing much we can do here.
      } finally if (writer != null) writer.close()
    }
  }

  Runtime.getRuntime.addShutdownHook(new Log.AtExit)
}
