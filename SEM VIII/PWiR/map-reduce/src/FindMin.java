import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;


public class FindMin {
    public static class MapClass extends MapReduceBase
    implements Mapper<Text, Text, LongWritable, LongWritable> {
        public void map(Text key, Text value,
            OutputCollector<LongWritable, LongWritable> output,
            Reporter reporter) throws IOException
        {
            LongWritable node = new LongWritable(new Long(key.toString()));
            LongWritable dist = new LongWritable(new Long(value.toString()));
            
            output.collect(node, dist);
        }
    }

    
    /* Reduces non-original distance values. */
    public static class CombinerClass extends MapReduceBase
    implements Reducer<LongWritable, LongWritable, LongWritable, LongWritable> {
        public void reduce(LongWritable key, Iterator<LongWritable> values,
                OutputCollector<LongWritable, LongWritable> output,
                Reporter reporter) throws IOException
        {
            long minimalDistance = Long.MAX_VALUE;
            while (values.hasNext()) {
                long distance = values.next().get();
                
                if (distance < 0) // original - propagate
                    output.collect(new LongWritable(key.get()), new LongWritable(distance));
                else // new, agregate
                    minimalDistance = Math.min(minimalDistance, distance);
            }
            output.collect(new LongWritable(key.get()), new LongWritable(minimalDistance));
        }
    }

    
    /* Finds minimal distance, counts relaxed nodes and marks revised nodes. */
    public static class ReduceClass extends MapReduceBase
    implements Reducer<LongWritable, LongWritable, LongWritable, Text> {
        static enum RecordCounters { RELAXED_NODES };
        
        public void reduce(LongWritable key, Iterator<LongWritable> values,
                   OutputCollector<LongWritable, Text> output,
                   Reporter reporter) throws IOException
        {
            long minimalDistance = Long.MAX_VALUE;
            long originalDistance = Long.MAX_VALUE;
            boolean done = false;
            
            while (values.hasNext()) {
                long dist = values.next().get();
                long normalizedDist = dist >= 0 ? dist : recodeValue(dist);
                minimalDistance = Math.min(minimalDistance, normalizedDist);
                
                if (dist < 0)
                    originalDistance = normalizedDist;
            }
            if (minimalDistance != originalDistance)
                reporter.incrCounter(RecordCounters.RELAXED_NODES, 1);
            else
                done = true;
            output.collect(new LongWritable(recodeValue(key.get())), new Text(minimalDistance + " " + done));
        }
        
        private static long recodeValue(long value) {
           return (value * -1) - 1;
        }
    }
}