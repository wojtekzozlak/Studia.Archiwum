import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.BooleanWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Writable;


public class NodeInfo implements Writable {
    public LongWritable dist;
    public BooleanWritable done;
    public LongArrayWritable neighbours; 

    public NodeInfo() {
       dist = new LongWritable(Long.MAX_VALUE);
       done = new BooleanWritable(false);
       neighbours = new LongArrayWritable();
    }
    
    public NodeInfo(LongWritable dist, BooleanWritable done, LongArrayWritable neighbours) {
        this.dist = dist;
        this.done = done;
        this.neighbours = neighbours;
    }

    public void write(DataOutput out) throws IOException {
      dist.write(out);
      done.write(out);
      neighbours.write(out);
    }

    public void readFields(DataInput in) throws IOException {
      dist.readFields(in);
      done.readFields(in);
      neighbours.readFields(in);
    }

    public String toString() {
      return dist.toString() + ", " + done.toString() + ", " + neighbours.toString();
    }
  }