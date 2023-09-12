using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using CppSharp;
using CppSharp.AST;
using CppSharp.Generators;

namespace dc
{
    /// <summary>
    /// The main base class for a generator-based tests project.
    /// </summary>
    public class GeneratorTest : ILibrary
    {
        private List<string> m_files;
        private List<string> m_dirs;

        public GeneratorTest(List<string> files, List<string> dirs)
        {
            m_files = files;
            m_dirs = dirs;
        }

        public override void Setup(Driver driver)
        {
            var options = driver.Options;
            options.GeneratorKind = GeneratorKind.CSharp;
            var module = options.AddModule("engine");
            foreach (var dir in m_dirs)
            {
                module.IncludeDirs.Add(dir);
                var files = Directory.EnumerateFiles(dir, "*.h");
                foreach (var file in files)
                {
                    module.Headers.Add(Path.GetFileName(file));
                }
            }

            //module.Headers.Add("Sample.h");
            //module.LibraryDirs.Add(@"C:\Sample\lib");
            //module.Libraries.Add("Sample.lib");
        }

        public override void Preprocess(Driver driver, ASTContext ctx)
        {
        }

        public override void Postprocess(Driver driver, ASTContext ctx)
        {
        }

        public override void SetupPasses(Driver driver)
        {
        }

        #region Helpers
        public static string GetTestsDirectory(string name)
        {
            var directory = new DirectoryInfo(
                Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));

            while (directory != null)
            {
                var path = Path.Combine(directory.FullName, "tests", name);

                if (Directory.Exists(path))
                    return path;

                path = Path.Combine(directory.FullName, "external", "CppSharp", "tests", name);

                if (Directory.Exists(path))
                    return path;

                directory = directory.Parent;
            }

            throw new Exception(string.Format(
                "Tests directory for project '{0}' was not found", name));
        }

        static string GetOutputDirectory()
        {
            string exePath = new Uri(Assembly.GetExecutingAssembly().CodeBase).LocalPath;
            var directory = Directory.GetParent(exePath);

            while (directory != null)
            {
                var path = Path.Combine(directory.FullName, "build");

                if (Directory.Exists(path))
                    return directory.FullName;

                directory = directory.Parent;
            }

            throw new Exception("Could not find tests output directory");
        }
        #endregion
    }
}
