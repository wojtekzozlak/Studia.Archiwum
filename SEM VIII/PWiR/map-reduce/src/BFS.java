import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.KeyValueTextInputFormat;
import org.apache.hadoop.mapred.RunningJob;
import org.apache.hadoop.mapred.TextOutputFormat;


public class BFS {
    
    private static JobConf createMergeJobConf(String graph, String in, String out) {
        Configuration conf = new Configuration();
        
        JobConf job = new JobConf(conf, MergeAndStep.class);
        
        job.setOutputKeyClass(LongWritable.class);
        job.setOutputValueClass(NodeInfo.class);
    
        job.setOutputFormat(TextOutputFormat.class);
            
        job.setMapperClass(MergeAndStep.MapClass.class);
        job.setReducerClass(MergeAndStep.ReduceClass.class);
            
        FileInputFormat.setInputPaths(job, new Path(graph), new Path(in));
        FileOutputFormat.setOutputPath(job, new Path(out));
    
        job.setJobName("GraphAndDistMerge");
        
        return job;
    }
    
    private static JobConf createStepJobConf(String in, String out) {
        Configuration conf = new Configuration();
        
        JobConf job = new JobConf(conf, FindMin.class);
        
        job.setMapOutputKeyClass(LongWritable.class);
        job.setMapOutputValueClass(LongWritable.class);
        
        job.setOutputKeyClass(LongWritable.class);
        job.setOutputValueClass(LongWritable.class);
        
        job.setInputFormat(KeyValueTextInputFormat.class);
        job.setOutputFormat(TextOutputFormat.class);
            
        job.setMapperClass(FindMin.MapClass.class);
        job.setReducerClass(FindMin.ReduceClass.class);
            
        FileInputFormat.setInputPaths(job, new Path(in));
        FileOutputFormat.setOutputPath(job, new Path(out));
    
        job.setJobName("FindMin");
        
        return job;
    }
    
    private static JobConf createFinalJobConf(String in, String out) {
        Configuration conf = new Configuration();
        
        JobConf job = new JobConf(conf, Finalize.class);
        
        job.setMapOutputKeyClass(LongWritable.class);
        job.setMapOutputValueClass(LongWritable.class);
        
        job.setOutputKeyClass(LongWritable.class);
        job.setOutputValueClass(Text.class);
        
        job.setInputFormat(KeyValueTextInputFormat.class);
        job.setOutputFormat(TextOutputFormat.class);
            
        job.setMapperClass(Finalize.MapClass.class);
        job.setReducerClass(Finalize.ReduceClass.class);
            
        FileInputFormat.setInputPaths(job, new Path(in));
        FileOutputFormat.setOutputPath(job, new Path(out));
    
        job.setJobName("Final");
        
        return job;
    }
    
    public static void main(String[] args) throws Exception {
        long iterations = Long.MAX_VALUE;
        if (args.length >= 3)
            iterations = Long.valueOf(args[2]);
        
        long i = 0;
        for (i = 1; i <= iterations; i++) {
            String prevStepB = "/user/mimuw/graph-tmp/step" + (i - 1) + "b";
            String stepA = "/user/mimuw/graph-tmp/step" + i + "a";
            String stepB = "/user/mimuw/graph-tmp/step" + i + "b";
            
            System.out.println(args[0] + " : " + prevStepB + " : " + stepA);
            JobConf mergeJob = createMergeJobConf(args[0], prevStepB, stepA);
            JobConf stepJob = createStepJobConf(stepA, stepB);
            
            JobClient.runJob(mergeJob);
            FileSystem.get(new Configuration()).delete(new Path(prevStepB), true);
            
            RunningJob runningStepJob = JobClient.runJob(stepJob);
            FileSystem.get(new Configuration()).delete(new Path(stepA), true);
            
            long relaxedNodes = runningStepJob.getCounters().getCounter(FindMin.ReduceClass.RecordCounters.RELAXED_NODES);
            if (relaxedNodes == 0) {
                i++;
                break;
            }
        }
        JobConf finalJob = createFinalJobConf("/user/mimuw/graph-tmp/step" + (i - 1) + "b", args[1]);
        JobClient.runJob(finalJob);
     }
}
