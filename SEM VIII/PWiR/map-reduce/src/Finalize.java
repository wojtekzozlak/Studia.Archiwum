import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;


public class Finalize {
    public static class MapClass extends MapReduceBase
    implements Mapper<Text, Text, LongWritable, LongWritable> {
        public void map(Text key, Text value,
            OutputCollector<LongWritable, LongWritable> output,
            Reporter reporter) throws IOException
        {
            StringTokenizer itr = new StringTokenizer(value.toString());
            LongWritable node = new LongWritable(new Long(key.toString()));
            LongWritable dist = new LongWritable(new Long(itr.nextToken()));
            
            output.collect(dist, decodeNode(node));
        }
        
        public static LongWritable decodeNode(LongWritable node) {
            return new LongWritable((node.get() * -1) - 1);
        }
    }

    
    public static class ReduceClass extends MapReduceBase
    implements Reducer<LongWritable, LongWritable, LongWritable, Text> {
        public void reduce(LongWritable key, Iterator<LongWritable> values,
                   OutputCollector<LongWritable, Text> output,
                   Reporter reporter) throws IOException
        {
            if (key.get() == Long.MAX_VALUE)
                return;
            
            ArrayList<Long> nodes = new ArrayList<Long>();
            while (values.hasNext()) {
                nodes.add(values.next().get());
            }
            
            StringBuilder builder = new StringBuilder();
            for (Long l : nodes) {
                if (builder.length() != 0)
                    builder.append(" ");
                builder.append(l);
            }

            output.collect(key, new Text(builder.toString()));
        }
    }
}